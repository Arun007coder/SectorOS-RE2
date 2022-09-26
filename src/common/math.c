#include "math.h"
#include "system.h"
#include "pit.h"
#include "logdisk.h"

static uint16_t cseed[3];

void seed(uint16_t seed)
{
    cseed[0] = seed * pit_ticks;
    cseed[1] = seed * pit_ticks;
    cseed[2] = seed * pit_ticks;
}

double floor(double x)
{
    return (int)x;
}

double fmod(double x, double y)
{
    return x - y * floor(x / y);
}

uint32_t rand()
{
    uint32_t x = cseed[0];
    uint32_t y = cseed[1];
    uint32_t z = cseed[2];
    x = x * 0x5DEECE66D + 0xB;
    y = y * 0x5DEECE66D + 0xB;
    z = z * 0x5DEECE66D + 0xB;
    cseed[0] = x;
    cseed[1] = y;
    cseed[2] = z;
    uint32_t ret = x ^ y ^ z;
    
    return ret;
}

void init_rand()
{
    seed(tick_before);
    for(int i = 0; i < pit_ticks; i++)
        rand();
    printl("Psuedo-random number generator initialized.\n");
}

uint32_t getBIT(uint32_t b, int num)
{
    return (b >> num) & 1;
}

uint32_t rand_range(uint32_t min, uint32_t max)
{
    return rand() % (max - min + 1) + min;
}

uint32_t get_bits(uint32_t value, uint32_t startBit, uint32_t X)
{
    unsigned  mask;
    mask = ((1 << X) - 1) << startBit;
    uint32_t isolatedXbits = value & mask;
    return isolatedXbits;

}