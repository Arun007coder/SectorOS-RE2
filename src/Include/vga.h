#ifndef __VGA_H__
#define __VGA_H__

#include "system.h"
#include "port.h"

#define VGA_FB_ADDRESS (0xB8000 + 0xC0000000)
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

#define VGA_BLACK 0
#define VGA_BLUE 1
#define VGA_GREEN 2
#define VGA_CYAN 3
#define VGA_RED 4
#define VGA_MAGENTA 5
#define VGA_BROWN 6
#define VGA_LIGHT_GREY 7
#define VGA_DARK_GREY 8
#define VGA_LIGHT_BLUE 9
#define VGA_LIGHT_GREEN 10
#define VGA_LIGHT_CYAN 11
#define VGA_LIGHT_RED 12
#define VGA_LIGHT_MAGENTA 13
#define VGA_LIGHT_BROWN 14
#define VGA_WHITE 15

void putchar(char c);
void puts(const char *s);

void clear();
void change_color(uint8_t fg, uint8_t bg);

#endif