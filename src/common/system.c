#include "system.h"
#include "printf.h"
#include "serial.h"
#include "vga.h"
#include "string.h"
#include "port.h"

void panic(const char* message, const char* file, uint32_t line)
{
    change_color(VGA_LIGHT_RED, VGA_BLACK);
    printf("PANIC: %s at %s:%d\n", message, file, line);
    while(1);
}

bool isAllZero(const void* data, size_t size)
{
    const uint8_t* ptr = (const uint8_t*)data;
    for(size_t i = 0; i < size; i++)
    {
        if(ptr[i] != 0)
            return false;
    }
    return true;
}

void kernel_panic(const char* message)
{
    change_color(VGA_LIGHT_RED, VGA_BLACK);
    printf("kernel panic: - %s\n", message);
    while(1);
}

void reboot()
{
    outb(0x64, 0xFE);
}