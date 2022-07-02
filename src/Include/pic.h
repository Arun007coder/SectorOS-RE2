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

#ifndef __PIC_H__
#define __PIC_H__

#include "system.h"
#include "port.h"

// PIC ports
// Master PIC
#define PIC1_COMMAND 0x20 
#define PIC1_DATA    0x21

// Slave PIC
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

// PIC command
#define PIC_EOI      0x20

void init_pic();
void pic_eoi(uint8_t irq);

#endif