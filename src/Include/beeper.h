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

#ifndef __BEEPER_H__
#define __BEEPER_H__

#include "system.h"
#include "port.h"
#include "pit.h"

// Frequency of notes
#define BEEP_A5  880
#define BEEP_B5  988
#define BEEP_C4  261
#define BEEP_D4  294
#define BEEP_E4  329
#define BEEP_F4  349
#define BEEP_G4  391
#define BEEP_A4  440
#define BEEP_B4  493
#define BEEP_C5  523
#define BEEP_D5  587
#define BEEP_E5  659
#define BEEP_F5  698
#define BEEP_G5  784

#define BEEP_C6  1047


void play_sound(uint32_t frequency);
void stop_sound();

void beep(uint32_t frequency, uint32_t duration);

#endif