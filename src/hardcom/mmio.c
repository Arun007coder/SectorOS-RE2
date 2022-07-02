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

#include "mmio.h"

uint8_t mmib(uint32_t addr)
{
    return *(uint8_t*)addr;
}

uint16_t mmiw(uint32_t addr)
{
    return *(uint16_t*)addr;
}

uint32_t mmil(uint32_t addr)
{
    return *(uint32_t*)addr;
}

void mmob(uint32_t addr, uint8_t val)
{
    *(uint8_t*)addr = val;
}

void mmow(uint32_t addr, uint16_t val)
{
    *(uint16_t*)addr = val;
}

void mmol(uint32_t addr, uint32_t val)
{
    *(uint32_t*)addr = val;
}