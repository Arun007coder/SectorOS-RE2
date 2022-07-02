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

#ifndef __sorfs_H
#define __sorfs_H

#include <stdint.h>

/*
* SORFS :- SectorOS sorfs v2.3
* A simple sorfs implementation for SectorOS
*/

#define MAX_BLOCKS 10
#define SORFS_SB_MAGIC 0xE09BAA
#define SORFS_BLOCK_MAGIC 0xE09CFF

#define SORFS_TYPE_REG 0 // Regular file
#define SORFS_TYPE_DIR 1 // Directory
#define SORFS_TYPE_LNK 2 // Symbolic link
#define SORFS_TYPE_FRE 3 // Free block

typedef struct 
{
    uint32_t magic;
    char name[32];
    uint32_t size;
    uint32_t total_blocks;
    uint32_t free_blocks;
}__attribute__((packed)) sorfs_superblock_t;

typedef struct sorfs_block
{
    uint32_t magic;
    char name[32];
    uint8_t type;
    uint32_t size;
    uint32_t offset;
} __attribute__((packed)) sorfs_block_t;

#endif