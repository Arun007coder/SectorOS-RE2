#include "tss.h"

tss_entry_t kernel_tss;

void init_tss(uint32_t idx, uint32_t kss, uint32_t kesp)
{
    uint32_t base = (uint32_t)&kernel_tss;
    uint32_t limit = sizeof(tss_entry_t);
    gdt_set_gate(idx, base, base + sizeof(tss_entry_t), 0xE9, 0);

    memset(&kernel_tss, 0, sizeof(tss_entry_t));
    kernel_tss.ss0 = kss;
    kernel_tss.esp0 = kesp;

    // Load userspace segments
    kernel_tss.cs = 0x0B;
    kernel_tss.ss = 0x13;
    kernel_tss.ds = 0x13;
    kernel_tss.es = 0x13;
    kernel_tss.fs = 0x13;
    kernel_tss.gs = 0x13;
    
    uint32_t cr3;
    asm volatile("mov %%cr3, %0" : "=r" (cr3));
    kernel_tss.cr3 = cr3;

    tss_flush();
}

void tss_set_kernel_stack(uint32_t kss, uint32_t kesp)
{
    kernel_tss.ss0 = kss;
    kernel_tss.esp0 = kesp;
}