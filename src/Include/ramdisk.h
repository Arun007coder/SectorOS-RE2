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

#ifndef __RAMDISK_H__
#define __RAMDISK_H__

#include <system.h>
#include <kheap.h>
#include <vfs.h>
#include <devfs.h>
#include <paging.h>

typedef struct ramdisk
{
    int id;

    uint32_t size;
    uint32_t start;
    uint32_t end;
}ramdisk_t;

void ramdisk_create(uint32_t start, uint32_t size);
void ramdisk_createFromFile(char* filename);

void ramdisk_destroy(vfs_node* v, char* null);

uint32_t ramdisk_read(FILE* ramdisk, uint32_t offset, uint32_t size, char *buffer);
uint32_t ramdisk_write(FILE* ramdisk, uint32_t offset, uint32_t size, char *buffer);

void ramdisk_open(FILE* ramdisk, uint32_t flags);
void ramdisk_close(FILE* ramdisk);

FILE* ramdisk_getVFSNode(ramdisk_t *ramdisk);

#endif