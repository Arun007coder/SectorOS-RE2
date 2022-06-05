#ifndef __SHELL_H__
#define __SHELL_H__

#include "system.h"
#include "printf.h"
#include "string.h"
#include "vga.h"
#include "serial.h"
#include "beeper.h"
#include "vfs.h"

#define SHELL_BUFFER_SIZE 256

void shell_run(uint8_t);
void init_shell();

#endif