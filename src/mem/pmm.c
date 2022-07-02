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

#include "pmm.h"

uint8_t* pmm_bitmap = (uint8_t*)(&end);
uint8_t* mem_start;
uint32_t total_blocks;
uint32_t pmm_bitmap_size;

void init_pmm(uint32_t mem_size)
{
    total_blocks = mem_size / BLOCK_SIZE;
    pmm_bitmap_size = total_blocks / BPP;
    if(pmm_bitmap_size * BPP < total_blocks)
        pmm_bitmap_size++;

    memset(pmm_bitmap, 0, pmm_bitmap_size);
    mem_start = (uint8_t*)BLOCK_ALIGN(((uint32_t)(pmm_bitmap + pmm_bitmap_size)));
    printf("Physical memory manager initialized.\n");
}

uint32_t alloc_block()
{
    uint32_t free_block = first_free_block();
    SETBIT(free_block);
    return free_block;
}

void free_block(uint32_t block)
{
    CLEARBIT(block);
}

uint32_t first_free_block()
{
    uint32_t i;
    for(i = 0; i < total_blocks; i++) {
        if(!ISSET(i))
            return i;
    }
    printE("PMM: Ran out of free blocks!\n");
    return (uint32_t) -1;
}

