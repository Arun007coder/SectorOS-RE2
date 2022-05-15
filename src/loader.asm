VM_START equ 0xC0000000
PDE_IDX equ (VM_START >> 22)
PSE_BIT equ 0x00000010
PG_BIT equ 0x80000000

section .data
align 4096
global TEMP_PAGE_DIR
TEMP_PAGE_DIR:
    dd 0x00000083
    times (PDE_IDX - 1) dd 0x00000000
    dd 0x00000083
    times(1024 - PDE_IDX - 1) dd 0x00000000

section .initial_stack, nobits
align 4
stack_bottom:
    resb 104856
stack_top:

section .text
global kernel_entry
global low_kernel_entry
low_kernel_entry equ (kernel_entry - VM_START)
kernel_entry:
    mov ecx, (TEMP_PAGE_DIR - VM_START)
    mov cr3, ecx

    mov ecx, cr4
    or ecx, PSE_BIT
    mov cr4, ecx

    mov ecx, cr0
    or ecx, PG_BIT
    mov cr0, ecx

    lea ecx, [high_kernel_entry]
    jmp ecx
high_kernel_entry:
    mov dword[TEMP_PAGE_DIR], 0
    invlpg[0]

    mov esp, stack_top
    extern kernelmain

    push eax
    add ebx, VM_START
    push ebx

    call kernelmain

loop:
    jmp loop