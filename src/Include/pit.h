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

typedef void (*PIT_CALLBACK)(void);

static uint32_t time_since_boot;

void init_pit(uint32_t frequency, isr_t callback);
void chfreq(uint32_t freq);
void wait(uint32_t ms);
void register_function(PIT_CALLBACK callback);

#endif