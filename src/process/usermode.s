.global switch_to_usermode

switch_to_usermode:
    cli
    mov $0x23, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    mov %esp, %eax
    pushl $0x23
    pushl %eax
    pushf
    pushl $0x1B
    push $1f
    iret
1:
    ret
