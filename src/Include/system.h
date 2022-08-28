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

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>
#include "multiboot.h"

#define KB 1024
#define MB (1024*KB)
#define GB (1024*MB)


#define MSECOND 1
#define SECOND 1000*MSECOND
#define MINUTE 60*SECOND
#define HOUR 60*MINUTE

#define HALT asm("cli"); asm("hlt");
#define ASSERT(b) ((b) ? (void)0 : panic(#b, __FILE__, __LINE__))

void panic(const char* message, const char* file, uint32_t line);
void kernel_panic(const char* message);
#define PANIC(msg) panic(msg, __FILE__, __LINE__);

#define PRINTBINARY(x) int i; for(i = 0; i < 32; i++) { if(x & (1 << (31 - i))) { putchar('1'); } else { putchar('0'); } }
#define SPRINTBINARY(x) int i; for(i = 0; i < 32; i++) { if(x & (1 << (31 - i))) { serial_putc('1'); } else { serial_putc('0'); } }

#define CPUID(in, a, b, c, d) asm("cpuid": "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "a"(in));

#ifndef KERNEL_BUILD
#define KERNEL_BUILD __DATE__ " " __TIME__
#endif

#ifndef COMPILER
#define COMPILER "unknown"
#endif

extern uint32_t end;
extern int paging_enabled;
extern bool kheap_enabled;
extern uint32_t mboot_addr;
extern uint32_t geteip();
extern uint32_t tick_before;

bool isAllZero(const void* data, size_t size);

#define KERNEL_BASE 0xC0000000
#define KERNEL_END  end

#define KERNEL_VERSION "5.0.0"
#define KERNEL_NAME "SectorOS-RE2"

#define SOSH_VERSION "0.1.8"

void print_cpuinfo();
void reboot();
void print_eflags(uint32_t eflags);

typedef struct CPUSTATE
{
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;

    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;

    uint32_t error;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;
}__attribute__((packed)) CPUSTATE_t;

typedef struct registers
{
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t ino, ecode;
    uint32_t eip, cs, eflags, useresp, ss;
} registers_t;

typedef struct register16
{
    uint16_t di;
    uint16_t si;
    uint16_t bp;
    uint16_t sp;
    uint16_t bx;
    uint16_t dx;
    uint16_t cx;
    uint16_t ax;

    uint16_t ds;
    uint16_t es;
    uint16_t fs;
    uint16_t gs;
    uint16_t ss;
    uint16_t eflags;
}register16_t;

uint16_t register_hl(uint8_t high, uint8_t low);
uint8_t register_h(uint16_t reg);
uint8_t register_l(uint16_t reg);

#endif