#include "bios32.h"

idt_ptr_t real_idt_ptr;
idt_ptr_t real_gdt_ptr;

void (*rebased_bios32_helper)() = (void*)0x7c00;

void init_bios32()
{
    gdt_set_gate(6, 0, 0xffffffff, 0x9A, 0x0f);

    gdt_set_gate(7, 0, 0xffffffff, 0x92, 0x0f);

    real_gdt_ptr.base = (uint32_t)gdt_entries;
    real_gdt_ptr.limit = sizeof(gdt_entries) - 1;

    real_idt_ptr.base = 0;
    real_idt_ptr.limit = 0x3ff;
}

void bios32_call(uint8_t int_num, register16_t* in_reg, register16_t* out_reg)
{
    void* new_codeBase = (void*)0x7c00;

    memcpy(&asm_gdt_entries, gdt_entries, sizeof(gdt_entries));

    real_gdt_ptr.base = (uint32_t)REBASE((&asm_gdt_entries));
    memcpy(&asm_gdt_ptr, &real_gdt_ptr, sizeof(real_gdt_ptr));

    memcpy(&asm_idt_ptr, &real_idt_ptr, sizeof(real_idt_ptr));

    memcpy(&asm_in_reg_ptr, in_reg, sizeof(register16_t));
    void * t = REBASE(&asm_in_reg_ptr);
    memcpy(&asm_intnum_ptr, &int_num, sizeof(uint8_t));

    uint32_t size = (uint32_t)bios32_helper_end - (uint32_t)bios32_helper;
    memcpy(new_codeBase, bios32_helper, size);
    if(size > 4096)
    {
        PANIC("Not enough space for bios32_helper");
    }

    rebased_bios32_helper();

    t = REBASE(&asm_out_reg_ptr);
    memcpy(out_reg, t, sizeof(register16_t));

    init_gdt();
    init_idt();
}