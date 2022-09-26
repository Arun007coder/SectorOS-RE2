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
    int CF = get_bits(eflags, 0, 2);
    int PF = get_bits(eflags, 2, 2);
    int AF = get_bits(eflags, 4, 2);
    int ZF = get_bits(eflags, 6, 1);
    int SF = get_bits(eflags, 7, 1);
    int TF = get_bits(eflags, 8, 1);
    int IF = get_bits(eflags, 9, 1);
    int DF = get_bits(eflags, 10, 1);
    int OF = get_bits(eflags, 11, 1);
    int IOPL = get_bits(eflags, 12, 2);
    int NT = get_bits(eflags, 14, 1);
    int RF = get_bits(eflags, 16, 1);
    int VM = get_bits(eflags, 17, 1);
    int AC = get_bits(eflags, 18, 1);
    int VIF = get_bits(eflags, 19, 1);
    int VIP = get_bits(eflags, 20, 1);
    int ID = get_bits(eflags, 21, 1);

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