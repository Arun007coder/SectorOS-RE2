#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include "system.h"
#include "printf.h"
#include "vga.h"
#include "beeper.h"
#include "isr.h"

void syscall_handler(registers_t reg);
void init_syscall();

#endif