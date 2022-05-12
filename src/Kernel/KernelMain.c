#include "system.h"
#include "printf.h"
#include "multiboot.h"
#include "gdt.h"
#include "idt.h"

void kernelmain(const multiboot_info_t* multiboot, uint32_t multiboot_m)
{
    init_gdt();
    init_idt();
    change_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    clear();
    printf("\nAbout kernel:\n");
    printf("Name: %s\n", KERNEL_NAME);
    printf("Version: %s\n", KERNEL_VERSION);
    printf("Build: %s\n", KERNEL_BUILD);
    printf("Kernel base: 0x%06x\n", KERNEL_BASE);
    printf("Kernel end: 0x%06x\n", KERNEL_END);
    printf("\n");
    printf("compiled with: %s\n", COMPILER);
    while(1);
}