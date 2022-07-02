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

#include "keyboard.h"

static keyboard_handler_t keyboard_handler = NULL;

void init_keyboard(keyboard_handler_t handler)
{
    if(handler == NULL)
    {
        printf("No keyboard handler given\n");
        return;
    }
    else
    {
        keyboard_handler = handler;
    }
    register_interrupt_handler(IRQ(1), &keyboard_callback);

    while(inb(KEYBOARD_COMMAND_PORT) & 0x1)
        inb(KEYBOARD_DATA_PORT);
    outb(KEYBOARD_COMMAND_PORT, 0xAE);
    outb(KEYBOARD_COMMAND_PORT, 0x20);
    uint8_t status = (inb(KEYBOARD_DATA_PORT) | 1) & ~0x10;
    outb(KEYBOARD_COMMAND_PORT, 0x60);
    outb(KEYBOARD_DATA_PORT, status);

    outb(KEYBOARD_DATA_PORT, 0xF4);
}

void change_keyboard_handler(keyboard_handler_t handler)
{
    if(handler == NULL)
    {
        printf("No keyboard handler given\n");
        handler = NULL;
        return;
    }
    else
    {
        keyboard_handler = handler;
    }
}

void keyboard_callback(registers_t *reg)
{
    uint8_t scancode;
    for(int i = 1000; i > 0; i++)
    {
        if((inb(KEYBOARD_COMMAND_PORT) & 1) == 0) continue;
        scancode = inb(KEYBOARD_DATA_PORT);
        break;
    }

    if(keyboard_handler != NULL)
    {
        keyboard_handler(scancode);
    }
    else
    {
        printf("No keyboard handler\n");
    }
}