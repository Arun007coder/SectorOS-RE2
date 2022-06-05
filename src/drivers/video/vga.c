#include "vga.h"

static uint16_t cursorx;
static uint16_t cursory;

static uint8_t  attributeByte;

static uint16_t* vidmem = (uint16_t*)VGA_FB_ADDRESS;

static void move_cursor()
{
    uint16_t pos = cursory * VGA_WIDTH + cursorx;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

static void scroll()
{
    uint16_t blank = 0x20 | (attributeByte << 8);

    if(cursory >= 25)
    {
        int i;
        for (i = 0*80; i < 24*80; i++)
        {
            vidmem[i] = vidmem[i+80];
        }

        for (i = 24*80; i < 25*80; i++)
        {
            vidmem[i] = blank;
        }
        cursory = 24;
    }
}

void putchar(char c)
{
    uint16_t attribute = attributeByte << 8;
    uint16_t *location;

    if (c == 0x08 && cursorx)
    {
        cursorx--;
    }
    else if (c == 0x09)
    {
        cursorx = (cursorx+8) & ~(8-1);
    }
    else if (c == '\r')
    {
        cursorx = 0;
    }
    else if (c == '\n')
    {
        cursorx = 0;
        cursory++;
    }
    else if(c >= ' ')
    {
        location = vidmem + (cursory*80 + cursorx);
       *location = c | attribute;
        cursorx++;
    }

    if (cursorx >= 80)
    {
        cursorx = 0;
        cursory ++;
    }

    scroll();
    move_cursor();
}

void puts(const char* str)
{
    int i = 0;
    while (str[i])
    {
        putchar(str[i++]);
    }
}


void clear()
{
    uint16_t blank = 0x20 | (attributeByte << 8);
    int i;
    for (i = 0; i < 80*25; i++)
    {
        vidmem[i] = blank;
    }
    cursory = 0;
    cursorx = 0;
    move_cursor();
}

void change_color(uint8_t fg, uint8_t bg)
{
    attributeByte = (bg << 4) | (fg & 0x0F);
}