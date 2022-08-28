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

#ifndef __XXD_H
#define __XXD_H

#include "system.h"
#include "printf.h"
#include "string.h"
#include "serial.h"

#define DUMP_COLS 16

/*
* @brief A function to dump the contents of a variable in a xxd like style
* @param ptr pointer to the data
* @param size size of the variable
*/
void xxd(void* ptr, uint32_t size);
void xxdf(void* ptr, uint32_t size);
void xxdl(void* ptr, uint32_t size);

#endif