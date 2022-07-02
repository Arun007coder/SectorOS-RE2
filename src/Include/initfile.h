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

#ifndef __INITFILE_H__
#define __INITFILE_H__

#define INITFILE_PATH "/init"

#include "system.h"
#include "string.h"
#include "list.h"
#include "vfs.h"
#include "printf.h"
#include "vesa.h"
#include "bitmap.h"
#include "targa.h"
#include "ext2.h"
#include "procfs.h"
#include "sorfs.h"
#include "mount.h"

void load_initfile();

void load_init(char* path);

#endif