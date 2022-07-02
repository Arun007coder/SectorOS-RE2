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

#ifndef __SHELL_H__
#define __SHELL_H__

#include "system.h"
#include "printf.h"
#include "string.h"
#include "vga.h"
#include "serial.h"
#include "beeper.h"
#include "vfs.h"
#include "mount.h"
#include "ramdisk.h"

#define SHELL_BUFFER_SIZE 256

void shell_run(uint8_t);
void init_shell();

#endif