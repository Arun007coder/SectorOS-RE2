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

#ifndef __MBR_H__
#define __MBR_H__

#include "system.h"
#include "string.h"
#include "vfs.h"
#include "devfs.h"
#include "printf.h"
#include "kheap.h"

#include "ext2.h"
#include "sorfs.h"

#define MBR_MAGIC 0xAA55

typedef struct PartitionTableEntry
{
    uint8_t bootable;
    uint8_t start_head;
    uint8_t start_sector : 6;
    uint16_t start_cylinder : 10;
    uint8_t partition_id;
    uint8_t end_head;
    uint8_t end_sector : 6;
    uint16_t end_cylinder : 10;
    
    uint32_t start_lba;
    uint32_t length;
} __attribute__((packed)) PTE_t;

typedef struct MBR
{
    uint8_t bootloader[440];
    uint32_t signature;
    uint16_t unused;
    
    PTE_t primaryPartition[4];
    
    uint16_t magicnumber;
} __attribute__((packed)) MBR_t;

typedef struct partition
{
    vfs_node* device;

    uint32_t num;

    uint32_t fs_type;

    uint8_t bootable;
    uint8_t partition_id;
    uint32_t start_lba;
    uint32_t length;
}partition_t;

void probe_partitions(char* device);

FILE* mbr_getVFSNode(partition_t* partition);

uint32_t mbr_read(FILE* node, uint32_t offset, uint32_t size, char* buffer);
uint32_t mbr_write(FILE* node, uint32_t offset, uint32_t size, char* buffer);

void mbr_open(FILE* node, uint32_t flags);
void mbr_close(FILE* node);

#endif