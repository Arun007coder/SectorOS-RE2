#include "system.h"
#include "isr.h"
#include "vga.h"
#include "printf.h"
#include "pic.h"

isr_t interrupt_handlers[256];

void register_interrupt_handler(int n, isr_t handler)
{
    if(n < 256)
    {
        interrupt_handlers[n] = handler;
    }
    printf("Registered interrupt handler for interrupt %d\n", n);
}

void irq_handler(registers_t regs)
{
    if(interrupt_handlers[regs.ino] != 0)
    {
        isr_t handler = interrupt_handlers[regs.ino];
        handler(&regs);
    }
    else
    {
        printf("No interrupt handler for interrupt %d\n", regs.ino);
    }
    pic_eoi(regs.ino);
}