/**
 * Copyright (C) 2022 Arun007coder
 * 
 * This file is part of SectorOS-RE2.
 * 
 * SectorOS-RE2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * SectorOS-RE2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with SectorOS-RE2.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "system.h"
#include "printf.h"
#include "multiboot.h"
#include "gdt.h"
#include "idt.h"
#include "pic.h"
#include "pit.h"
#include "serial.h"
#include "shell.h"
#include "keyboard.h"
#include "pmm.h"
#include "kheap.h"
#include "paging.h"
#include "vfs.h"
#include "pci.h"
#include "rtc.h"
#include "beeper.h"
#include "am79c973.h"
#include "devfs.h"
#include "ata.h"
#include "ext2.h"
#include "procfs.h"
#include "syscall.h"
#include "bios32.h"
#include "vesa.h"
#include "bitmap.h"
#include "logdisk.h"
#include "targa.h"
#include "ata_pio.h"
#include "initfile.h"
#include "ramdisk.h"
#include "sorfs.h"
#include "mbr.h"
#include "mount.h"
#include "tss.h"
#include "usermode.h"
#include "draw.h"
#include "font.h"
#include "netinterface.h"
#include "netutils.h"
#include "ethernet.h"
#include "arp.h"
#include "ipv4.h"
#include "icmp.h"
#include "udp.h"
#include "dhcp.h"
#include "rtl8139.h"
#include "apm.h"
#include "nulldev.h"
#include "randomdev.h"
#include "process.h"
#include "syscall.h"
#include "spinlock.h"

uint32_t mboot_addr;
uint32_t tick_before;

#define USE_VIDEO 0
#define USE_NETWORK 0

void usermode_puts(const char* str)
{
    while(*str)
    {
        SYSCALL_PUTCHAR(*str)
        str++;
    }
}

void uputc(char c)
{
    SYSCALL_PUTCHAR(c);
}

void uprintf(const char* fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    vsprintf(NULL, uputc, fmt, arg);
    va_end(arg);
}

uint32_t usermode_rand_range(uint32_t min, uint32_t max)
{
    uint32_t rand = 0;
    SYSCALL_RAND(rand)
    return rand % (max - min + 1) + min;
}

int win = 0;
int lose = 0;
int tie = 0;

char rps_buf[16];
int rps_buf_index;

void rps_handler(char key, int isCTRL, int isALT, uint8_t scancode)
{
    if(key != '\n')
    {
        if(key != '\300')
        {
            SYSCALL_PUTCHAR(key)
            rps_buf[rps_buf_index] = key;
            rps_buf_index++;
        }
    }
    else if(key == '\n')
    {
        rps_buf[rps_buf_index] = '\0';
        SYSCALL_PUTCHAR('\n')

        usermode_puts("Won: ");
        usermode_puts(itoa_r(win, 10));
        SYSCALL_PUTCHAR('\n')

        usermode_puts("Lost: ");
        usermode_puts(itoa_r(lose, 10));
        SYSCALL_PUTCHAR('\n')

        usermode_puts("Tie: ");
        usermode_puts(itoa_r(tie, 10));
        SYSCALL_PUTCHAR('\n')

        int option = atoi(rps_buf);
        int cpu_option = rand_range(0, 2);

        usermode_puts("Your option: ");
        usermode_puts((option == 0 ? "Rock" : (option == 1 ? "Paper" : (option == 2 ? "Scissor" : (option == 3 ? "Exit" : "Unknown")))));
        SYSCALL_PUTCHAR('\n')

        usermode_puts("CPU option: ");
        usermode_puts((cpu_option == 0 ? "Rock" : (cpu_option == 1 ? "Paper" : (cpu_option == 2 ? "Scissor" : "Unknown"))));
        SYSCALL_PUTCHAR('\n')

        switch(option)
        {
        case 0: //Rock
            switch(cpu_option)
            {
            case 0: //Rock
                tie++;
                usermode_puts("Tie\n");
                break;
            case 1: //Paper
                lose++;
                usermode_puts("You lose!\n");
                break;
            case 2: //Scissor
                win++;
                usermode_puts("You win!\n");
                break;
            };
            break;
        case 1: //Paper
            switch(cpu_option)
            {
            case 0: //Rock
                win++;
                usermode_puts("You win!\n");
                break;
            case 1: //Paper
                tie++;
                usermode_puts("Tie!\n");
                break;
            case 2: //Scissor
                lose++;
                usermode_puts("You lose!\n");
                break;
            };
            break;
        case 2: //Scissor
            switch(cpu_option)
            {
            case 0: //Rock
                lose++;
                usermode_puts("You lose!\n");
                break;
            case 1: //Paper
                win++;
                usermode_puts("You win!\n");
                break;
            case 2: //Scissor
                tie++;
                usermode_puts("Tie!\n");
                break;
            };
            break;
        case 3:
            SYSCALL_EXIT(0)
            break;
        };

        usermode_puts("\n");

        memset(rps_buf, 0, 16);
        rps_buf_index  = 0;
    }
}

void rps()
{
    usermode_puts("A simple rock paper scissor game.\nOptions:\n");
    usermode_puts("\t0: rock\n\t1: paper\n\t2: scissor\n\t3: exit\n");
    SYSCALL_ATTACH(rps_handler)
    while(1);
}

char *kbhb;
int kbhb_index = 0;

char *c_dir;

void handler(char key, int isCTRL, int isALT, uint8_t scancode)
{
    SYSCALL_PUTCHAR(key);
    if(scancode == 0x3B)
    {
        usermode_puts(kbhb);
        SYSCALL_PUTCHAR('r');
    }
    else if(key != '\n')
    {
        if (key != '\300')
        {
            if(key == '\b')
            {
                kbhb_index--;
            }
            else
            {
                kbhb[kbhb_index] = key;
                kbhb_index++;
            }
        }
    }
    else
    {
        kbhb[kbhb_index] = '\0';
        if(strcmp(kbhb, "exit") == 0)
        {
            SYSCALL_EXIT(0)
        }
        else if(strncmp(kbhb, "echo", 4) == 0)
        {
            char* str = kbhb + 5;
            usermode_puts(str);
        }
        else if(strcmp(kbhb, "shutdown") == 0)
        {
            apm_shutdown();
        }
        else if(strcmp(kbhb, "rand") == 0)
        {
            int rng = 0;
            SYSCALL_RAND(rng)
            usermode_puts(itoa_r(rng, 10));
        }
        else if(strcmp(kbhb, "rps") == 0)
        {
            memset(kbhb, 0, 256);
            create_process_from_routine("rps", rps, TASK_TYPE_KERNEL);
        }
        else if(strcmp(kbhb, "clear") == 0)
        {
            clear();
        }
        else if (strcmp(kbhb, "licence") == 0)
        {
            printf("Licence: GPLv3\n");
            printf("Copyright (C) 2022 Arun007coder\n");
        }
        else if(strcmp(kbhb, "help") == 0)
        {
            usermode_puts("help:\n\t-exit: to exit the shell\n\t-echo [string]: to echo the given string\n\t-shutdown: To shutdown the computer\n\t-rand: To get a random positive number\n\t-rps: To play rock paper scissor\n\t-clear: To clear the screen\n\t-licence: To print the licence\n\t-uname -[anbvh]: To get information about the kernel\n\t-chpr [pid]: To change to given pid\n\t-listp: To list the processes running\n\t-ls [dir]: To get the list of files in given directory\n\t-cd [dir]: To change the current working directory\n\t-cat [filepath]: To print the contents of given file\n\t-help: To print this information\n");
        }
        else if (strstr(kbhb, "ls") != NULL)
        {
            char *filename = strstr(kbhb, "ls") + 3;
            filename[strlen(filename)] = '\0';
            if (filename[0] == '\0')
            {
                VFS_db_listdir(c_dir);
                serial_printf("c_dir: %s\n", c_dir);
            }
            else
                VFS_db_listdir(filename);
        }
        else if (strstr(kbhb, "cd") != NULL)
        {
            char *filename = strstr(kbhb, "cd") + 3;
            filename[strlen(filename)] = '\0';
            memset(c_dir, 0, 512);
            if (filename[0] == '\0')
            {
                strcpy(c_dir, "/");
            }
            else
            {
                strcpy(c_dir, filename);
            }
        }
        else if (strstr(kbhb, "cat") != NULL)
        {
            char *filename = strstr(kbhb, "cat") + 4;
            filename[strlen(filename)] = '\0';
            vfs_node *node;
            if (filename[0] != '/')
            {
                char *path = (char *)kmalloc(strlen(c_dir) + strlen(filename) + 1);
                strcpy(path, c_dir);
                strcat(path, "/");
                strcat(path, filename);
                node = file_open(path, 0);
                kfree(path);
            }
            else
            {
                node = file_open(filename, 0);
            }

            if (node != NULL)
            {
                uint32_t size = VFS_getFileSize(node);
                char *buffer = (char *)kmalloc((sizeof(char) * (size ? size : 1024)));
                memset(buffer, 0, size);
                VFS_read(node, 0, size, buffer);
                printf("\n%s", buffer);
                kfree(buffer);
            }
            else
            {
                printf("File %s could not be opened\n", filename);
            }
        }
        else if (strncmp(kbhb, "uname", 5) == 0)
        {
            char *arg = strdup(kbhb + 6);
            if (arg[0] != '-')
            {
                printf("%s version %s %s x86\n", KERNEL_NAME, KERNEL_VERSION, KERNEL_BUILD);
            }
            else
            {
                if (arg[1] == 'a')
                {
                    printf("%s version %s %s x86\n", KERNEL_NAME, KERNEL_VERSION, KERNEL_BUILD);
                }
                else if (arg[1] == 'n')
                {
                    printf("%s\n", KERNEL_NAME);
                }
                else if (arg[1] == 'b')
                {
                    printf("%s\n", KERNEL_BUILD);
                }
                else if (arg[1] == 'v')
                {
                    printf("%s\n", KERNEL_VERSION);
                }
                else if (arg[1] == 'h')
                {
                    printf("Usage: uname [-anbvh]\n");
                }
            }
        }
        else if(strncmp(kbhb, "chpr", 4) == 0)
        {
            pid_t pid = atoi((kbhb + 5));
            change_process(pid);
        }
        else if(strcmp(kbhb, "listp") == 0)
        {
            list_process();
        }
        else
        {
            usermode_puts("Unknown command: ");
            usermode_puts(kbhb);
        }

        SYSCALL_PUTCHAR('\n');
        uprintf("$%s> ", c_dir);
        memset(kbhb, 0, 256);
        kbhb_index = 0;
    }
}

void init()
{
    usermode_puts("\nSOSH v0.2.0\nThis is a basic shell running in kernel mode!\nRun help to get help\n\n");
    usermode_puts("$/> ");
    c_dir = strdup("/");
    SYSCALL_MALLOC(kbhb, 256)
    spinlock_lock(kbhb);
    SYSCALL_ATTACH(handler)
    while(1);
}

void kernelmain(const multiboot_info_t* multiboot, uint32_t multiboot_m)
{
    change_color(VGA_LIGHT_GREEN, VGA_BLACK);
    clear();

    if(multiboot_m != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        kernel_panic("Invalid multiboot magic number.\n");
    }

    if(isAllZero(multiboot, sizeof(multiboot_info_t)))
    {
        kernel_panic("Invalid multiboot info.\n");
    }

    mboot_addr = (uint32_t)multiboot;
    init_gdt();

    init_idt();
    init_pic();
    init_bios32();

    init_syscall();

    init_pmm(1096 * MB);
    init_paging();
    init_kheap(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, KHEAP_MAX_ADDRESS);

    init_logdisk(MB);
    printl("GDT initialized.\n");
    printl("IDT initialized.\n");
    printl("PIC initialized.\n");
    printl("Starting BIOS32 process.\n");
    printl("BIOS32 initialized.\n");
    printl("System call initialized.\n");
    printl("Physical memory manager initialized.\n");
    printl("Paging initialized.\n");
    printl("Kernel heap initialized.\n");
    printl("Logdisk initialized.\n");

    init_vfs();

    init_procfs();
    printl("PROCFS initialized.\n");

    init_devfs();
    printl("DEVFS initialized. Mounted on /dev/\n");

    logdisk_mount();

    init_pit(SECOND, NULL);
    printl("PIT initialized.\n");

    tick_before = pit_ticks;

    init_shell();

    init_processManager();

    enable_interrupts();

    printl("Interrupts enabled.\n");

    init_serial(COM1);
    printl("Serial port initialized. Using serial port COM1 at port 0x%04x\n", COM1);
    serial_printf("[COM1] Testing serial port...\n");

    init_networkInterfaceManager();
    printl("[NEM] Network Interface Manager Initalized...\n");

    init_pci();
    printl("PCI initialized.\n");
    char* pci_info = (char*)kmalloc(1024);
    procfs_enumerate_pci_devices(pci_info);
    printl("%s\n", pci_info);
    kfree(pci_info);

    procfs_mount("/proc");
    printl("PROCFS mounted on /proc\n");

    FILE* se = serial_getvfsnode();
    devfs_add(se);

    init_ata_pio();
    printl("ATA initialized.\n");

    init_nulldev();
    init_randomdev();

    mount("/dev/apio0p0", "/");

    uint8_t second, minute, hour, day, month;
    uint32_t year;
    rtc_read(&second, &minute, &hour, &day, &month, &year);
    printf("%d/%d/%d %d:%d:%d\n", month, day, year, hour, minute, second);

    printl("Kernel Successfully Initialized.\n");

    printl("Updating ESP in the kernel tss...\n");

    uint32_t esp;
    asm volatile("mov %%esp, %0" : "=r"(esp));

    tss_set_kernel_stack(0x10, esp);

    printl("ESP successfully updated in the tss as 0x%06x.\n", esp);

    printl("Starting init process from /init...\n");

    printl("Init completed..\n");
    
    seed(pit_ticks);

    for(int i = 0; i < pit_ticks; i++)
    {
        rand();
    }

    create_process_from_routine("sosh", init, TASK_TYPE_KERNEL);

#if USE_VIDEO
    init_vesa();

    canvas_t canvas = canvas_create(vesa_getXResolution(), vesa_getYResolution(), vesa_getFramebuffer());

    set_fill_color(rgb(176, 20, 85));

    for(int i = 0; i < canvas.width; i++)
    {
        for(int j = 0; j < canvas.height; j++)
        {
            set_pixel(&canvas, rand(), i, j);
        }
    }

    veprintf(&canvas, "Hello Graphical world!\nWelcome to SectorOS RE2 %s\nGoing online soon...\n", KERNEL_VERSION);
#endif

    while(1);
}