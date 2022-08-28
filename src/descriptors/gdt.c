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

#include "gdt.h"
#include "tss.h"

extern void gdt_flush(uint32_t);

void flush_gdt();

gdt_entry_t gdt_entries[GDT_ENTRY_COUNT];
gdt_ptr_t   gdt_ptr;

void init_gdt()
{
    gdt_ptr.limit = sizeof(gdt_entry_t) * GDT_ENTRY_COUNT - 1;
    gdt_ptr.base  = (uint32_t)&gdt_entries;

    gdt_set_gate(0, 0, 0, 0, 0);                // 0x00: Null segment
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // 0x08: Code segment
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // 0x10: Data segment
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // 0x18: User mode code segment
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // 0x20: User mode data segment

    gdt_flush((uint32_t)&gdt_ptr);

    init_tss(5, 0x10, 0);
}

void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;

    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access       = access;
}

void flush_gdt()
{
    asm volatile("lgdt (%0)" : : "r" (&gdt_ptr));
}