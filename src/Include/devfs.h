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

#ifndef __DEVFS_H_
#define __DEVFS_H_

#include "system.h"
#include "vfs.h"
#include "string.h"
#include "kheap.h"

#define MAX_DEVICES 64

void init_devfs();
void devfs_add(FILE* device);
void devfs_remove(char* name);
FILE* devfs_rnode();

void devfs_mount(char* mountpoint);

void devfs_unlink(FILE* parent, char* name);

vfs_node *devfs_finddir(vfs_node *node, char *name);
DirectoryEntry* devfs_readdir(vfs_node *node, uint32_t index);
void devfs_open(vfs_node *node, uint32_t flags);
void devfs_close(vfs_node *node);
char** devfs_listdir(vfs_node* node);

#endif