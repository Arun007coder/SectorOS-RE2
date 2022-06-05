#include "mmio.h"

uint8_t mmib(uint32_t addr)
{
    return *(uint8_t*)addr;
}

uint16_t mmiw(uint32_t addr)
{
    return *(uint16_t*)addr;
}

uint32_t mmil(uint32_t addr)
{
    return *(uint32_t*)addr;
}

void mmob(uint32_t addr, uint8_t val)
{
    *(uint8_t*)addr = val;
}

void mmow(uint32_t addr, uint16_t val)
{
    *(uint16_t*)addr = val;
}

void mmol(uint32_t addr, uint32_t val)
{
    *(uint32_t*)addr = val;
}