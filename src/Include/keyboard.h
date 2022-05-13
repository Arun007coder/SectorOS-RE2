#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "system.h"
#include "string.h"
#include "port.h"
#include "printf.h"
#include "isr.h"

#define KEYBOARD_COMMAND_PORT 0x64
#define KEYBOARD_DATA_PORT 0x60

typedef void (*keyboard_handler_t)(uint8_t);

void init_keyboard(keyboard_handler_t handler);
void keyboard_callback(registers_t regs);

#endif