// Copyright (C) 2022 Arun007coder
// 
// This file is part of SectorOS-RE2.
// 
// SectorOS-RE2 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// SectorOS-RE2 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with SectorOS-RE2.  If not, see <http://www.gnu.org/licenses/>.

#ifndef __BIOS_32_H__
#define __BIOS_32_H__

#include <system.h>
#include <idt.h>
#include <gdt.h>
#include <paging.h>
#include <kheap.h>
#include <printf.h>

extern void bios32_helper();
extern void bios32_helper_end();
extern void * asm_gdt_entries;
extern void * asm_gdt_ptr;
extern void * asm_idt_ptr;
extern void * asm_in_reg_ptr;
extern void * asm_out_reg_ptr;
extern void * asm_intnum_ptr;

#define REBASE(x) (void*)(0x7c00 + (void*)x - (uint32_t)bios32_helper)

#define BIOS_GRAPHICS_SERVICE 0x10

void init_bios32();
void bios32_call(uint8_t int_num, register16_t* in_reg, register16_t* out_reg);

#endif