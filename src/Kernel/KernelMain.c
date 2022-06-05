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

uint32_t mboot_addr;

void kernelmain(const multiboot_info_t* multiboot, uint32_t multiboot_m)
{
    mboot_addr = (uint32_t)multiboot;
    change_color(VGA_LIGHT_GREEN, VGA_BLACK);
    clear();
    init_gdt();
    init_idt();
    init_pic();

    init_pmm(1096 * MB);
    init_kheap(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, KHEAP_MAX_ADDRESS);
    init_paging();

    init_shell();

    init_pit(SECOND, NULL);
    enable_interrupts();
    init_serial(COM1);

    init_pci();

    init_vfs();
    devfs_init();
    init_procfs();
    procfs_mount("/proc");

    FILE* se = serial_getvfsnode();
    devfs_add(se);

    init_ata();
    EXT2_init("/dev/ata0m", "/");

    FILE* test = file_open("/test.txt", 0);
    if(test != NULL)
    {
        char* buffer = (char*)kmalloc(VFS_getFileSize(test));
        VFS_read(test, 0, VFS_getFileSize(test), buffer);
        serial_printf("%s\n", buffer);
    }
    else
    {
        serial_printf("Failed to open file\n");
    }

    uint8_t second, minute, hour, day, month;
    uint32_t year;
    rtc_read(&second, &minute, &hour, &day, &month, &year);
    printf("%d/%d/%d %d:%d:%d\n", month, day, year, hour, minute, second);

    printf("Welcome to %s!\n", KERNEL_NAME);
    printf("Type \"help\" for a list of commands.\n\n");

    printf("#> ");

    while(1);
}