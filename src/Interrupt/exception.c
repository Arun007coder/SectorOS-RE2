#include "system.h"
#include "isr.h"
#include "vga.h"
#include "printf.h"

char* exception_messages[] =
{
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void exception_handler(registers_t cps)
{
    if(cps.ino < IRQ_BASE)
    {
        change_color(VGA_COLOR_RED, VGA_COLOR_BLACK);
        printf("KERNEL PANIC. EXCEPTION %d: %s\n", cps.ino, exception_messages[cps.ino]);
        printf("Extended stack pointer = 0x%06x\n", cps.esp);
        printf("Extended instruction pointer  = 0x%06x\n", cps.eip);
        printf("Code segment selector = 0x%06x\n", cps.cs);
        printf("Extended flags = 0x%06x\n", cps.eflags);
        printf("Error code = 0x%06x\n", cps.ecode);
        printf("\n");
        printf("Registers:\n");
        printf("EAX = 0x%06x\n", cps.eax);
        printf("EBX = 0x%06x\n", cps.ebx);
        printf("ECX = 0x%06x\n", cps.ecx);
        printf("EDX = 0x%06x\n", cps.edx);
        printf("ESI = 0x%06x\n", cps.esi);
        printf("EDI = 0x%06x\n", cps.edi);
        printf("EBP = 0x%06x\n", cps.ebp);
        change_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        for(;;);
    }
    if(interrupt_handlers[cps.ino] != 0)
    {
        isr_t handler = interrupt_handlers[cps.ino];
        handler(cps);
    }
    else
    {
        printf("No interrupt handler for interrupt %d\n", cps.ino);
    }
}