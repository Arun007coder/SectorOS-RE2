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
#include "serial.h"
#include "vga.h"
#include "string.h"
#include "port.h"

void panic(const char* message, const char* file, uint32_t line)
{
    change_color(VGA_LIGHT_RED, VGA_BLACK);
    printf("PANIC: %s at %s:%d\n", message, file, line);
    while(1);
}

bool isAllZero(const void* data, size_t size)
{
    const uint8_t* ptr = (const uint8_t*)data;
    for(size_t i = 0; i < size; i++)
    {
        if(ptr[i] != 0)
            return false;
    }
    return true;
}

void kernel_panic(const char* message)
{
    change_color(VGA_LIGHT_RED, VGA_BLACK);
    printf("kernel panic: - %s\n", message);
    while(1);
}

void reboot()
{
    outb(0x64, 0xFE);
}

uint16_t register_hl(uint8_t high, uint8_t low)
{
    return (high << 8) | low;
}

uint8_t register_h(uint16_t reg)
{
    return (reg >> 8) & 0xFF;
}

uint8_t register_l(uint16_t reg)
{
    uint8_t low = reg & 0xFF;
    return low;
}

void print_eflags(uint32_t eflags)
{
    int CF = (eflags & 0x0001);
    int PF = (eflags & 0x0004);
    int AF = (eflags & 0x0010);
    int ZF = (eflags & 0x0040);
    int SF = (eflags & 0x0080);
    int TF = (eflags & 0x0100);
    int IF = (eflags & 0x0400);
    int DF = (eflags & 0x0800);
    int OF = (eflags & 0x1000);
    int IOPL = (eflags & 0x3000) >> 12;
    int NT = (eflags & 0x4000);
    int RF = (eflags & 0x10000);
    int VM = (eflags & 0x20000);
    int AC = (eflags & 0x40000);
    int VIF = (eflags & 0x80000);
    int VIP = (eflags & 0x100000);
    int ID = (eflags & 0x200000);

    printf("flags: [ ");
    printf("%s", CF ? "Carry " : "");
    printf("%s", PF ? "Parity " : "");
    printf("%s", AF ? "Adjust " : "");
    printf("%s", ZF ? "Zero " : "");
    printf("%s", SF ? "Sign " : "");
    printf("%s", TF ? "Trap " : "");
    printf("%s", IF ? "Interrupt " : "");
    printf("%s", DF ? "Direction " : "");
    printf("%s", OF ? "Overflow " : "");
    printf("%s", IOPL ? "IO privilege level " : "");
    printf("%s", NT ? "Nested " : "");
    printf("%s", RF ? "Resume " : "");
    printf("%s", VM ? "Virtual8086 " : "");
    printf("%s", AC ? "Alignment " : "");
    printf("%s", VIF ? "Virtual Interrupt " : "");
    printf("%s", VIP ? "Virtual Interrupt pending " : "");
    printf("%s", ID ? "CPUID " : "");
    printf("]\n");
}