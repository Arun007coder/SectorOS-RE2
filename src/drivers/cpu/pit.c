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

#include "pit.h"

uint32_t pit_ticks = 0;
uint32_t pit_freq = 0;
uint32_t pit_freq_before = 0;

list_t* wakeup_list = NULL;

void timer_callback(registers_t *reg)
{
    pit_ticks++;
    time_since_boot = pit_ticks / pit_freq;

    foreach(t, wakeup_list)
    {
        wakeup_t* w = (wakeup_t*)t->val;
        w->callback();
    }
}

void init_pit(uint32_t freq, isr_t callback)
{
    register_interrupt_handler(IRQ(0), &timer_callback);
    wakeup_list = list_create();

    chfreq(freq);
}

void chfreq(uint32_t freq)
{
    pit_freq_before = pit_freq;
    uint32_t divisor = (PIT_FREQUENCY / freq);
    pit_freq = divisor;
    outb(PIT_COMMAND, 0x36);
    outb(PIT_CHANNEL0, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL0, (uint8_t)((divisor >> 8) & 0xFF));
}

uint32_t get_freq()
{
    return pit_freq;
}

void wait(uint32_t ms)
{
    uint32_t start = pit_ticks;
    uint32_t end = (start + ms) - 1;
    while(pit_ticks < end)
    {
        asm volatile("hlt");
    }
}

void register_function(PIT_CALLBACK callback, double seconds)
{
    uint32_t tick = pit_ticks + seconds * pit_freq;

    wakeup_t* wakeup = kmalloc(sizeof(wakeup_t));
    wakeup->callback = callback;
    wakeup->seconds = seconds;
    wakeup->ticks = tick;
    list_insert_front(wakeup_list, wakeup);
}