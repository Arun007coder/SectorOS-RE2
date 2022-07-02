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

#ifndef __PMM_H__
#define __PMM_H__

#include "system.h"
#include "printf.h"
#include "string.h"
#include "vga.h"

#define BLOCK_SIZE 4096
#define BPP 8

#define SETBIT(i) pmm_bitmap[i / BPP] = pmm_bitmap[i / BPP] | (1 << (i % BPP))
#define CLEARBIT(i) pmm_bitmap[i / BPP] = pmm_bitmap[i / BPP] & (~(1 << (i % BPP)))
#define ISSET(i) ((pmm_bitmap[i / BPP] >> (i % BPP)) & 0x1)
#define GET_BUCKET32(i) (*((uint32_t*) &pmm_bitmap[i / 32]))

#define BLOCK_ALIGN(i) (((i) & 0xFFFFF000) + 0x1000)

void init_pmm(uint32_t size);
uint32_t alloc_block();
void free_block(uint32_t block);
uint32_t first_free_block();

#endif