#ifndef __MMIO_H__
#define __MMIO_H__

// Memory-mapped I/O

#include "system.h"
#include "string.h"

uint8_t mmib(uint32_t address);
uint16_t mmiw(uint32_t address);
uint32_t mmil(uint32_t address);

void mmob(uint32_t address, uint8_t value);
void mmow(uint32_t address, uint16_t value);
void mmol(uint32_t address, uint32_t value);

#endif