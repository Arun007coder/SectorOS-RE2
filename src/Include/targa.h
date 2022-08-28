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

#ifndef __TARGA_H__
#define __TARGA_H__

#include "system.h"
#include "string.h"
#include "vesa.h"

typedef struct targa
{
    uint32_t width;
    uint32_t height;
    uint32_t size;

    uint32_t* img_buffer;
}targa_t;

targa_t* targa_parse(char* filename);
void targa_display(char* name);

#endif