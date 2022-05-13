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

uint32_t mboot_addr;

void kernelmain(const multiboot_info_t* multiboot, uint32_t multiboot_m)
{
    mboot_addr = (uint32_t)multiboot;
    change_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    clear();
    init_gdt();
    init_idt();
    init_pic();
    init_pit(SECOND, NULL);
    enable_interrupts();
    init_serial(COM1);
    printf("Welcome to %s!\n", KERNEL_NAME);
    printf("Type \"help\" for a list of commands.\n\n");

    printf("#> ");
    init_keyboard(shell_run);

    while(1);
}