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

#ifndef __MATH_H
#define __MATH_H

#include "system.h"
#include "stdint.h"

#define abs(a) (((a) < 0)?-(a):(a))
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define sign(x) ((x < 0) ? -1 :((x > 0) ? 1 : 0))

#define is_power_of_two(x) (((x) & ((x) - 1)) == 0)

void seed(uint16_t seed);
uint32_t rand();
uint32_t rand_range(uint32_t min, uint32_t max);

uint32_t getBIT(uint32_t b, int num);

#endif