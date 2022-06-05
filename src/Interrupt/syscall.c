#include "syscall.h"

void syscall_handler(registers_t reg)
{
    switch(reg.eax)
    {
    case 0:
        putchar(reg.ebx);
        break;
    case 1:
        printf("%s", (char*)reg.ebx);
        break;
    case 2:
        beep(1000, 100);
        break;
    }
}

void init_syscall()
{
    register_interrupt_handler(0x80, syscall_handler);
}