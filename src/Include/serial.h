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

#ifndef __SERIAL_H
#define __SERIAL_H

#include "system.h"
#include "printf.h"
#include "port.h"
#include "pit.h"
#include "vfs.h"
#include "devfs.h"
#include "logdisk.h"

#define COM1 0x3F8
#define COM2 0x2F8
#define COM3 0x3E8
#define COM4 0x2E8

void init_serial(uint16_t port);
void serial_putc(char c);
void serial_puts(char *str);
char serial_getc();
void serial_printf(char *fmt, ...);
FILE* serial_getvfsnode();
bool getSerialInit();

#endif