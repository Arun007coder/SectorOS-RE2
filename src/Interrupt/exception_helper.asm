; Copyright (C) 2022 Arun007coder
; 
; This file is part of SectorOS-RE2.
; 
; SectorOS-RE2 is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
; 
; SectorOS-RE2 is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License
; along with SectorOS-RE2.  If not, see <http://www.gnu.org/licenses/>.

%macro EXCE_NOECODE 1
    [GLOBAL exception%1]
    exception%1:
        cli
        push byte 0
        push %1
        jmp exception_common
%endmacro

%macro EXCE_ECODE 1
    [GLOBAL exception%1]
    exception%1:
        cli
        push %1
        jmp exception_common
%endmacro

EXCE_NOECODE 0
EXCE_NOECODE 1
EXCE_NOECODE 2
EXCE_NOECODE 3
EXCE_NOECODE 4
EXCE_NOECODE 5
EXCE_NOECODE 6
EXCE_NOECODE 7
EXCE_ECODE 8
EXCE_NOECODE 9

EXCE_ECODE 10
EXCE_ECODE 11
EXCE_ECODE 12
EXCE_ECODE 13
EXCE_ECODE 14
EXCE_NOECODE 15
EXCE_NOECODE 16
EXCE_NOECODE 17
EXCE_NOECODE 18
EXCE_NOECODE 19
EXCE_NOECODE 20
EXCE_NOECODE 21
EXCE_NOECODE 22
EXCE_NOECODE 23
EXCE_NOECODE 24
EXCE_NOECODE 25
EXCE_NOECODE 26
EXCE_NOECODE 27
EXCE_NOECODE 28
EXCE_NOECODE 29
EXCE_NOECODE 30
EXCE_NOECODE 31
EXCE_NOECODE 128


[extern exception_handler]

exception_common:
    pusha
    mov ax, ds
    push eax

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call exception_handler

    pop ebx
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx

    popa
    add esp, 0x08

    sti
    iret