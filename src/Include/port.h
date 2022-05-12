#ifndef __PORT_H__
#define __PORT_H__

#include "system.h"

void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);

#endif