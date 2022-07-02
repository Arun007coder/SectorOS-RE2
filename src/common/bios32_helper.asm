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

[bits 32]

global bios32_helper
global bios32_helper_end

global asm_gdt_ptr
global asm_gdt_entries
global asm_idt_ptr
global asm_in_reg_ptr
global asm_out_reg_ptr
global asm_intnum_ptr


extern gdt_init
extern idt_init
extern new_gdt_entries;
extern new_gdt_ptr;
extern new_idt_ptr;
extern new_reg_ptr;
extern new_intnum_ptr;

%define REBASE(x)                              (((x) - bios32_helper) + 0x7c00)
%define GDTENTRY(x)                            ((x) << 3)
%define CODE32                                 GDTENTRY(1)
%define DATA32                                 GDTENTRY(2)
%define CODE16                                 GDTENTRY(6)
%define DATA16                                 GDTENTRY(7)


PG_BIT_OFF equ 0x7fffffff
PG_BIT_ON equ 0x80000000

section .text
bios32_helper: use32
    pusha
    mov edx, esp
    cli
    mov ecx, cr0
    and ecx, PG_BIT_OFF
    mov cr0, ecx
    xor ecx, ecx
    mov ebx, cr3
    mov cr3, ecx

    lgdt [REBASE(asm_gdt_ptr)]

    lidt [REBASE(asm_idt_ptr)]
   
    jmp CODE16:REBASE(protected_mode_16)

protected_mode_16:use16
    mov ax, DATA16
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov eax, cr0
    and  al,  ~0x01
    mov cr0, eax

    jmp 0x0:REBASE(real_mode_16)

real_mode_16:use16
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov sp, 0x8c00

    sti

    pusha
    mov cx, ss
    push cx
    mov cx, gs
    push cx
    mov cx, fs
    push cx
    mov cx, es
    push cx
    mov cx, ds
    push cx
    pushf

    mov ax, sp
    mov edi, temp_esp
    stosw

    mov esp, REBASE(asm_in_reg_ptr)

    popa

    mov sp, 0x9c00

    db 0xCD

asm_intnum_ptr:
    db 0x00
    mov esp, REBASE(asm_out_reg_ptr)
    add sp, 28

    pushf
    mov cx, ss
    push cx
    mov cx, gs
    push cx
    mov cx, fs
    push cx
    mov cx, es
    push cx
    mov cx, ds
    push cx
    pusha

    mov esi, temp_esp
    lodsw
    mov sp, ax

    popf
    pop cx
    mov ds, cx
    pop cx
    mov es, cx
    pop cx
    mov fs, cx
    pop cx
    mov gs, cx
    pop cx
    mov ss, cx
    popa

    mov eax, cr0
    inc eax
    mov cr0, eax
    jmp CODE32:REBASE(protected_mode_32)

protected_mode_32:use32
    mov ax, DATA32
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov cr3, ebx

    mov ecx, cr0
    or ecx, PG_BIT_ON
    mov cr0, ecx
    
    mov esp, edx

    sti
    popa
    ret

padding:
    db 0x0
    db 0x0
    db 0x0

asm_gdt_entries:
    times 64 dw 0

asm_gdt_ptr:
    dd 0x00000000
    dd 0x00000000

asm_idt_ptr:
    dd 0x00000000
    dd 0x00000000

asm_in_reg_ptr:
    times 14 dw 0

asm_out_reg_ptr:
    dd 0xaaaaaaaa
    dd 0xaaaaaaaa
    dd 0xaaaaaaaa
    dd 0xaaaaaaaa
    dd 0xaaaaaaaa
    dd 0xaaaaaaaa
    dd 0xaaaaaaaa

temp_esp:
    dw 0x0000
    
bios32_helper_end: