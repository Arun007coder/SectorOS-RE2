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
#define PANIC(msg) panic(msg, __FILE__, __LINE__);

#define CPUID(in, a, b, c, d) __asm__("cpuid": "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "a"(in));
#ifndef KERNEL_BUILD
#define KERNEL_BUILD "2022-01-01"
#endif

#ifndef COMPILER
#define COMPILER "unknown"
#endif

extern uint32_t end;
extern int paging_enabled;
extern bool kheap_enabled;
extern uint32_t mboot_addr;

#define KERNEL_BASE 0xC0000000
#define KERNEL_END  end

#define KERNEL_VERSION "2.0.0"
#define KERNEL_NAME "SectorOS-RE2"

void print_cpuinfo();

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

#endif