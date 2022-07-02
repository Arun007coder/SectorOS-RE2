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

uint32_t mboot_addr;

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

    init_shell();
    printl("Shell initialized.\n");

    init_pit(SECOND, NULL);
    printl("PIT initialized.\n");

    enable_interrupts();

    printl("Interrupts enabled.\n");

    init_serial(COM1);
    printl("Serial port initialized. Using serial port COM1 at port 0x%04x\n", COM1);

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
    init_ata();

    printl("ATA initialized.\n");

    uint8_t second, minute, hour, day, month;
    uint32_t year;
    rtc_read(&second, &minute, &hour, &day, &month, &year);
    printf("%d/%d/%d %d:%d:%d\n", month, day, year, hour, minute, second);

    mount("/dev/apio0p0", "/");

    load_initfile();

    printl("Kernel Successfully Initialized.\n");

    printf("/#> ");

    while(1);
}