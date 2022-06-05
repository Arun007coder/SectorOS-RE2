#ifndef __RTC_H_
#define __RTC_H_

#include "system.h"
#include "port.h"

#define CMOS_COMMAND 0x70
#define CMOS_DATA    0x71
#define CURRENT_YEAR 2021

// Registers
#define RTC_REG_SECONDS 0x00
#define RTC_REG_MINUTES 0x02
#define RTC_REG_HOURS   0x04
#define RTC_REG_DAYS    0x07
#define RTC_REG_MONTHS  0x08
#define RTC_REG_YEARS   0x09
#define RTC_REG_B       0x0B

typedef struct
{
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint32_t year;
} rtc_time_t;

void rtc_read(uint8_t* second, uint8_t *minute, uint8_t *hour, uint8_t *day, uint8_t *month, uint32_t *year);
void rtc_read_time(rtc_time_t *time);

#endif