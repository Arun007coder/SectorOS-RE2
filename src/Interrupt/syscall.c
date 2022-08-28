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

#include "syscall.h"
#include "vesa.h"

void * syscall_table[10] = {
    putchar,
    printf,
    VFS_create,
    vesa_change_mode,
    file_open,
    VFS_close,
    VFS_read,
    VFS_write,
    VFS_getFileSize,
    malloc,
};

void syscall_handler(registers_t *reg)
{
    if(reg->eax >= 20) return;
    void * system_api = syscall_table[reg->eax];
    int ret;
    asm volatile (" \
    push %1; \
    push %2; \
    push %3; \
    push %4; \
    push %5; \
    call *%6; \
    pop %%ebx; \
    pop %%ebx; \
    pop %%ebx; \
    pop %%ebx; \
    pop %%ebx; \
    " : "=a" (ret) : "r" (reg->edi), "r" (reg->esi), "r" (reg->edx), "r" (reg->ecx), "r" (reg->ebx), "r" (system_api));

    reg->eax = ret;
}

void init_syscall()
{
    register_interrupt_handler(0x80, syscall_handler);
}