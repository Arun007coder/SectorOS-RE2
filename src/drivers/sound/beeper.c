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

#include "beeper.h"

void play_sound(uint32_t frequency)
{
	uint32_t Div;
	uint8_t tmp;

	Div = 1193180 / frequency;
	outb(0x43, 0xb6);
	outb(0x42, (Div & 0xFF));
	outb(0x42, (Div >> 8) & 0xFF);

	tmp = inb(0x61);
    if (tmp != (tmp | 3))
    {
		outb(0x61, tmp | 3);
	}
}

void stop_sound()
{
	uint8_t tmp = inb(0x61) & 0xFC;
	outb(0x61, tmp);
}

void beep(uint32_t freq, uint32_t duration)
{
    play_sound(freq);
    wait(duration);
    stop_sound();
}