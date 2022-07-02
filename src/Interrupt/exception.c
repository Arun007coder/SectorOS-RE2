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
    if(cps.ino < 32)
    {
        change_color(VGA_RED, VGA_BLACK);
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
        change_color(VGA_WHITE, VGA_BLACK);
    }

    if(interrupt_handlers[cps.ino] != 0)
    {
        isr_t handler = interrupt_handlers[cps.ino];
        handler(&cps);
    }
    else if(cps.ino == 0x80)
    {
        printE("Syscall Handler is not yet initialized.\n");
    }
    else
    {
        PANIC("Unhandled exception!");
    }
}