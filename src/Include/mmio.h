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

#ifndef __MMIO_H__
#define __MMIO_H__

// Memory-mapped I/O

#include "system.h"
#include "string.h"

uint8_t mmib(uint32_t address);
uint16_t mmiw(uint32_t address);
uint32_t mmil(uint32_t address);

void mmob(uint32_t address, uint8_t value);
void mmow(uint32_t address, uint16_t value);
void mmol(uint32_t address, uint32_t value);

#endif