#include "system.h"
#include "printf.h"
#include "serial.h"
#include "vga.h"
#include "string.h"

void panic(const char* message, const char* file, uint32_t line)
{
    change_color(VGA_LIGHT_RED, VGA_BLACK);
    printf("PANIC: %s at %s:%d\n", message, file, line);
    while(1);
}