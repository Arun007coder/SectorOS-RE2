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

#ifndef __PIT_H
#define __PIT_H

#include "system.h"
#include "port.h"
#include "isr.h"
#include "printf.h"

#define PIT_FREQUENCY 1193180
#define PIT_CHANNEL0 0x40
#define PIT_CHANNEL1 0x41
#define PIT_CHANNEL2 0x42
#define PIT_COMMAND 0x43

typedef void (*wakeup_callback)(void);

extern uint32_t pit_ticks;
extern uint32_t pit_freq;

static uint32_t time_since_boot;

typedef struct 
{
    wakeup_callback callback;
    double seconds;
    uint32_t ticks;
}wakeup_info_t;

void init_pit(uint32_t frequency, isr_t callback);
void chfreq(uint32_t freq);
uint32_t get_freq();
void wait(uint32_t ms);
void register_function(wakeup_callback callback, double seconds);

#endif