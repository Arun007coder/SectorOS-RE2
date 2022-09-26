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
#include "process.h"

void * syscall_table[MAX_SYSCALLS] = {
    putchar, //0
    exit, //1
    attach_handler, //2
    file_open, //3
    VFS_close, //4
    syscall_vfs_read, //5
    syscall_vfs_write, //6
    VFS_getFileSize, //7
    VFS_create, //8
    malloc, //9
    free, //10
    realloc, //11
    rand, //12
    wait, //13
    syscall_create_process, //14
    change_process, //15
    clear, //16
};

void syscall_create_process(char* name, void* entrypoint)
{
    create_process_from_routine(name, entrypoint, TASK_TYPE_USER);
}

void syscall_vfs_read(vfs_node* file, syscall_rwo_t* options)
{
    VFS_read(file, options->offset, options->size, options->buffer);
}

void syscall_vfs_write(vfs_node* file, syscall_rwo_t* options)
{
    VFS_read(file, options->offset, options->size, options->buffer);
}

void syscall_handler(registers_t *reg)
{
    if(reg->eax >= MAX_SYSCALLS) return;
    void * system_api = syscall_table[reg->eax];
    int ret;
    memcpy(&saved_context, reg, sizeof(registers_t));
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