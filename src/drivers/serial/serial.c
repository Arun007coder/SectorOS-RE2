/**
 * Copyright (C) 2022 Arun007coder
 * 
 * This file is part of SectorOS-RE2.
 * 
 * SectorOS-RE2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * SectorOS-RE2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with SectorOS-RE2.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "serial.h"

static uint16_t Port = 0;
static bool isSerialInit = false;

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
    isSerialInit = true;
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
    if(c == '\n')
        serial_putc('\r');
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

uint32_t serial_read(vfs_node *file, uint32_t offset, uint32_t size, char *buffer)
{
    for(uint32_t i = 0; i < size; i++)
    {
        buffer[i] = serial_getc();
    }
    return size;
}

uint32_t serial_write(vfs_node *file, uint32_t offset, uint32_t size, char *buffer)
{
    serial_puts(buffer);
    serial_putc('\n');
}

void serial_open(vfs_node *file, uint32_t flags)
{
    printf("SERIAL opened\n");
}

void serial_close(vfs_node *file)
{
    printf("SERIAL closed\n");
    kfree(file);
}

FILE* serial_getvfsnode()
{
    FILE* node = (FILE*)kmalloc(sizeof(FILE));
    strcpy(node->name, "serial");
    node->flags = FS_CHARDEVICE;
    node->read = serial_read;
    node->write = serial_write;
    node->open = serial_open;
    node->close = serial_close;
    return node;
}



void serial_printf(char* format, ...)
{
    va_list args;
    va_start(args, format);
    vsprintf(NULL, serial_putc, format, args);
    va_end(args);
}

bool getSerialInit()
{
    return isSerialInit;
}