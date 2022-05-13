#include "serial.h"

static uint16_t Port = 0;

void init_serial(uint16_t port)
{
    Port = port;

    outb(port + 1, 0x00);
    outb(port + 3, 0x80);
    outb(port + 0, 0x03);
    outb(port + 1, 0x00);
    outb(port + 3, 0x03);
    outb(port + 2, 0xC7);
    outb(port + 4, 0x0B);
    outb(port + 4, 0x1E);
    outb(port + 0, 0xAE);

    if(inb(port + 0) != 0xAE)
    {
        printf("Serial port %x is not working\n", port);
        return;
    }

    outb(port + 4, 0x0F);
    printf("Serial port %x is working\n", port);
}

int serial_received()
{
    return inb(Port + 5) & 1;
}

char serial_getc()
{
    while(serial_received() == 0);
    return inb(Port);
}

int is_transmit_empty()
{
    return inb(Port + 5) & 0x20;
}

void serial_putc(char c)
{
    while(is_transmit_empty() == 0);
    outb(Port, c);
}

void serial_puts(char* str)
{
    while(*str)
    {
        serial_putc(*str);
        str++;
    }
}

void serial_printf(char* format, ...)
{
    va_list args;
    va_start(args, format);
    vsprintf(NULL, serial_putc, format, args);
    va_end(args);
}