// Copyright (C) 2022 Arun007coder
// 
// This file is part of SectorOS-RE2.
// 
// SectorOS-RE2 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// SectorOS-RE2 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with SectorOS-RE2.  If not, see <http://www.gnu.org/licenses/>.

#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include "system.h"
#include "printf.h"
#include "vga.h"
#include "beeper.h"
#include "isr.h"

#define MAX_SYSCALLS 18

#define SYSCALL(ret, a, b, c, d) asm volatile("int $0x80": "=a"(ret) : "a"(a), "b"(b), "c"(c), "d"(d));
#define SYSCALL2(a, b, c, d) asm volatile("int $0x80": : "a"(a), "b"(b), "c"(c), "d"(d));

#define SYSCALL_PUTCHAR(c) SYSCALL2(0, c, 0, 0)
#define SYSCALL_EXIT(code)    SYSCALL2(1, code, 0, 0)
#define SYSCALL_ATTACH(handler)  SYSCALL2(2, handler, 0, 0)
#define SYSCALL_FILE_OPEN(filename, flags, ret) SYSCALL(ret, 3, filename, flags, 0)
#define SYSCALL_VFS_CLOSE(file) SYSCALL2(4, file, 0, 0)
#define SYSCALL_VFS_READ(file, options) SYSCALL2(5, file, options, 0)
#define SYSCALL_VFS_WRITE(file, options) SYSCALL2(6, file, options, 0)
#define SYSCALL_VFS_GETFILESIZE(file, size) SYSCALL(size, 7, file, 0, 0)
#define SYSCALL_VFS_CREATE(path, permissions) SYSCALL2(8, path, permissions, 0)
#define SYSCALL_MALLOC(ptr, size) SYSCALL(ptr, 9, size, 0, 0)
#define SYSCALL_FREE(ptr) SYSCALL2(10, ptr, 0, 0)
#define SYSCALL_REALLOC(ret_ptr, ptr, size) SYSCALL(ret_ptr, 11, ptr, size, 0)
#define SYSCALL_RAND(ret) SYSCALL(ret, 12, 0, 0, 0)
#define SYSCALL_WAIT(ms) SYSCALL2(13, ms, 0, 0)
#define SYSCALL_CREATE_PROCESS_FUN(name, routine) SYSCALL2(14, name, routine, 0)
#define SYSCALL_CHANGE_PROCESS(pid) SYSCALL2(15, pid, 0, 0)
#define SYSCALL_CLEAR SYSCALL2(16, 0, 0, 0)

typedef struct syscall_rw_options
{
    uint32_t offset;
    uint32_t size;
    char* buffer;
}syscall_rwo_t;

void syscall_vfs_read(vfs_node* file, syscall_rwo_t* options);
void syscall_vfs_write(vfs_node* file, syscall_rwo_t* options);
void syscall_create_process(char* name, void* entrypoint);

void syscall_handler(registers_t* reg);
void init_syscall();

/*
Syscall table:
| NN |-----------------------|
| 00 |putchar                |
| 01 |exit                   |
| 02 |attach_handler         |
| 03 |file_open              |
| 04 |VFS_close              |
| 05 |VFS_read               |
| 06 |VFS_write              |
| 07 |VFS_getFileSize        |
| 08 |VFS_create             |
| 09 |malloc                 |
| 10 |free                   |
| 11 |realloc                |
| 12 |rand                   |
| 13 |wait                   |
| 14 |create_process         |
| 15 |change_process         |
| 16 |clear                  |
------------------------------

*/

#endif