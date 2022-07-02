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