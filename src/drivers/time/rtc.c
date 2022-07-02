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

#include "rtc.h"

static int century_register = 0;

uint8_t rtc_read_reg(int reg)
{
    outb(CMOS_COMMAND, reg);
    return inb(CMOS_DATA);
}

uint32_t rtc_get_update_in_progres_flag()
{
    outb(CMOS_COMMAND, 0x0A);
    return inb(CMOS_DATA) & 0x80;
}

void rtc_read(uint8_t* second, uint8_t *minute, uint8_t *hour, uint8_t *day, uint8_t *month, uint32_t *year)
{
    uint8_t century;
    uint8_t last_second;
    uint8_t last_minute;
    uint8_t last_hour;
    uint8_t last_day;
    uint8_t last_month;
    uint8_t last_year;
    uint8_t last_century;
    uint8_t registerB;

    while(rtc_get_update_in_progres_flag());
    (*second) = rtc_read_reg(RTC_REG_SECONDS);
    (*minute) = rtc_read_reg(RTC_REG_MINUTES);
    (*hour) = rtc_read_reg(RTC_REG_HOURS);
    (*day) = rtc_read_reg(RTC_REG_DAYS);
    (*month) = rtc_read_reg(RTC_REG_MONTHS);
    (*year) = rtc_read_reg(RTC_REG_YEARS);
    if(century_register != 0)
    {
        century = rtc_read_reg(century_register);
    }

    do
    {
        last_second = (*second);
        last_minute = (*minute);
        last_hour = (*hour);
        last_day = (*day);
        last_month = (*month);
        last_year = (*year);
        last_century = century;
        
        while(rtc_get_update_in_progres_flag());
        (*second) = rtc_read_reg(RTC_REG_SECONDS);
        (*minute) = rtc_read_reg(RTC_REG_MINUTES);
        (*hour) = rtc_read_reg(RTC_REG_HOURS);
        (*day) = rtc_read_reg(RTC_REG_DAYS);
        (*month) = rtc_read_reg(RTC_REG_MONTHS);
        (*year) = rtc_read_reg(RTC_REG_YEARS);
        if(century_register != 0)
        {
            century = rtc_read_reg(century_register);
        }
    }
    while(last_second != (*second) || last_minute != (*minute) || last_hour != (*hour) || last_day != (*day) || last_month != (*month) || last_year != (*year) || last_century != century);

    registerB = rtc_read_reg(RTC_REG_B);

    if(!(registerB & 0x04))
    {
        (*second) = ((*second) & 0x0F) + (((*second) / 16) * 10);
        (*minute) = ((*minute) & 0x0F) + (((*minute) / 16) * 10);
        (*hour) = ( ((*hour) & 0x0F) + ((((*hour) & 0x70) / 16) * 10) ) | ((*hour) & 0x80);
        (*day) = ((*day) & 0x0F) + (((*day) / 16) * 10);
        (*month) = ((*month) & 0x0F) + (((*month) / 16) * 10);
        (*year) = ((*year) & 0x0F) + (((*year) / 16) * 10);
        if(century_register != 0)
        {
            century = (century & 0x0F) + ((century / 16) * 10);
        }
    }

    if(!(registerB & 0x02) && ((*hour) & 0x80))
    {
        (*hour) = (((*hour) & 0x7F) + 12) % 24;
    }

    if(century_register != 0)
    {
        (*year) += century * 100;
    }
    else
    {
        (*year) += (CURRENT_YEAR / 100) * 100;
        if((*year) < CURRENT_YEAR)
        {
            (*year) += 100;
        }
    }
}

void rtc_read_time(rtc_time_t *time)
{
    rtc_read(&time->second, &time->minute, &time->hour, &time->day, &time->month, &time->year);
}