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

#ifndef __LOGDISK_H__
#define __LOGDISK_H__

#include "system.h"
#include "vfs.h"
#include "devfs.h"
#include "kheap.h"
#include "printf.h"
#include "string.h"

void init_logdisk(uint32_t size);

void logdisk_chsize(uint32_t new_size);

void logdisk_putc(char c);
void logdisk_puts(char *str);
void printl(char* fmt, ...);

char logdisk_getc(uint32_t offset);
void logdisk_gets(uint32_t offset, char* buf, uint32_t size);

void printlog();

void logdisk_mount();

FILE* logdisk_getVFSNode();
#endif