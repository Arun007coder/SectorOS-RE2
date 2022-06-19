#ifndef __SERIAL_H
#define __SERIAL_H

#include "system.h"
#include "printf.h"
#include "port.h"
#include "pit.h"
#include "vfs.h"
#include "devfs.h"
#include "logdisk.h"

#define COM1 0x3F8
#define COM2 0x2F8
#define COM3 0x3E8
#define COM4 0x2E8

void init_serial(uint16_t port);
void serial_putc(char c);
void serial_puts(char *str);
char serial_getc();
void serial_printf(char *fmt, ...);
FILE* serial_getvfsnode();
bool getSerialInit();

#endif