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