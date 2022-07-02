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
#include "pic.h"

isr_t interrupt_handlers[256];

void register_interrupt_handler(int n, isr_t handler)
{
    if(n < 256)
    {
        interrupt_handlers[n] = handler;
    }
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

void enable_interrupts()
{
    asm volatile("sti");
    printf("Interrupts enabled\n");
}

void disable_interrupts()
{
    asm volatile("cli");
    printf("Interrupts disabled\n");
}