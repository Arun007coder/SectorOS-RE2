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

#include "ramdisk.h"

uint32_t id = 0;

void ramdisk_create(uint32_t start, uint32_t size)
{
    ramdisk_t* ramdisk = kmalloc(sizeof(ramdisk_t));
    ramdisk->start = start;
    ramdisk->size = size;
    ramdisk->end = start + size;

    ramdisk->id = id++;

    devfs_add(ramdisk_getVFSNode(ramdisk));
    printf("[RAMDISK] Created ramdisk #%d at 0x%06x with size %sB\n", ramdisk->id, start, itoa_r(size, 10));
}

void ramdisk_createFromFile(char* filename)
{
    FILE* f = file_open(filename, 0);
    if(f == NULL)
    {
        printf("[RAMDISK] Failed to open file %s\n", filename);
        return;
    }
    
    uint32_t size = VFS_getFileSize(f);
    char* buffer = kmalloc(size);
    VFS_read(f, 0, size, buffer);

    ramdisk_create(((uint32_t)buffer), size);
}

uint32_t ramdisk_getFileSize(vfs_node* v)
{
    ramdisk_t* ramdisk = (ramdisk_t*)v->device;
    return ramdisk->size;
}

void ramdisk_destroy(vfs_node* v, char* null)
{
    if(null != NULL)
    {
        printf("[RAMDISK] Invalid argument\n");
        return;
    }
    if(v == NULL)
    {
        return;
    }
    ramdisk_t* ramdisk = (ramdisk_t*)v->device;
    printf("[RAMDISK] Destroyed ramdisk #%d at 0x%06x with size %sB\n", ramdisk->id, ramdisk->start, itoa_r(ramdisk->size, 10));
    kfree((char*)ramdisk->start);
}

FILE* ramdisk_getVFSNode(ramdisk_t* r)
{
    FILE* f = kmalloc(sizeof(FILE));

    strcpy(f->name, "ramdisk");
    strcat(f->name, itoa_r(r->id, 10));
    f->device = r;

    f->read = ramdisk_read;
    f->write = ramdisk_write;
    f->open = ramdisk_open;
    f->close = ramdisk_close;
    f->get_filesize = ramdisk_getFileSize;
    f->unlink = ramdisk_destroy;

    f->size = r->size;

    return f;
}

uint32_t ramdisk_read(FILE* node, uint32_t offset, uint32_t size, char* buffer)
{
    ramdisk_t* ramdisk = (ramdisk_t*)node->device;

    if(offset + size > ramdisk->size)
        return -1;

    memcpy(buffer, (void*)(ramdisk->start + offset), size);
    return size;
}

uint32_t ramdisk_write(FILE* node, uint32_t offset, uint32_t size, char* buffer)
{
    ramdisk_t* ramdisk = (ramdisk_t*)node->device;

    if(offset + size > ramdisk->size)
        return -1;

    memcpy((void*)(ramdisk->start + offset), buffer, size);
    return size;
}

void ramdisk_open(FILE* node, uint32_t flags)
{
    return;
}

void ramdisk_close(FILE* node)
{
    return;
}