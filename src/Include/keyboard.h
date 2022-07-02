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

#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "system.h"
#include "string.h"
#include "port.h"
#include "printf.h"
#include "isr.h"

#define KEYBOARD_COMMAND_PORT 0x64
#define KEYBOARD_DATA_PORT 0x60

typedef void (*keyboard_handler_t)(uint8_t);

void init_keyboard(keyboard_handler_t handler);
void keyboard_callback(registers_t *regs);
void change_keyboard_handler(keyboard_handler_t handler);

#endif