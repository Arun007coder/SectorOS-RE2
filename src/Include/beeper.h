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