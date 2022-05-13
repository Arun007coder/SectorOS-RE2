#include "pit.h"

static uint32_t pit_ticks = 0;
static uint32_t pit_freq = 0;
static uint32_t pit_freq_before = 0;

static uint32_t callbacks_index = 0;
static PIT_CALLBACK callbacks[256];

void timer_callback(registers_t reg)
{
    pit_ticks++;
    if(callbacks_index > 0)
    {
        for(uint32_t i = 0; i < callbacks_index; i++)
        {
            callbacks[i]();
        }
    }
}

void init_pit(uint32_t freq, isr_t callback)
{
    if(callback == NULL)
    {
        printf("No handlers given\n");
        register_interrupt_handler(IRQ(0), &timer_callback);
    }

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

void wait(uint32_t ms)
{
    uint32_t start = pit_ticks;
    uint32_t end = (start + ms) - 1;
    while(pit_ticks < end)
    {
        asm volatile("hlt");
    }
}

void register_function(PIT_CALLBACK callback)
{
    callbacks[callbacks_index] = callback;
    callbacks_index++;
}