#ifndef __ISR_H
#define __ISR_H

#include "system.h"

typedef void (*isr_t)(registers_t);
extern isr_t interrupt_handlers[256];

void register_interrupt_handler(int num, isr_t handler);
void enable_interrupts();
void disable_interrupts();

#define IRQ_BASE 0x20
#define IRQ(n) (IRQ_BASE + n)


// Pointer to the Interrupts in idt_helper.asm
extern void exception0();     // 0x00: Exception 0: Divide Error
extern void exception1();     // 0x01: Exception 1: Debug
extern void exception2();     // 0x02: Exception 2: Non Maskable Interrupt
extern void exception3();     // 0x03: Exception 3: Breakpoint
extern void exception4();     // 0x04: Exception 4: Overflow
extern void exception5();     // 0x05: Exception 5: Bound Range Exceeded
extern void exception6();     // 0x06: Exception 6: Invalid Opcode
extern void exception7();     // 0x07: Exception 7: Device Not Available
extern void exception8();     // 0x08: Exception 8: Double Fault
extern void exception9();     // 0x09: Exception 9: Coprocessor Segment Overrun [386][GPF]
extern void exception10();    // 0x0A: Exception 10: Invalid TSS
extern void exception11();    // 0x0B: Exception 11: Segment Not Present
extern void exception12();    // 0x0C: Exception 12: Stack Fault
extern void exception13();    // 0x0D: Exception 13: General Protection Fault
extern void exception14();    // 0x0E: Exception 14: Page Fault
extern void exception15();    // 0x0F: Exception 15: Reserved
extern void exception16();    // 0x10: Exception 16: x87 FPU Floating-Point Error
extern void exception17();    // 0x11: Exception 17: Alignment Check
extern void exception18();    // 0x12: Exception 18: Machine Check
extern void exception19();    // 0x13: Exception 19: SIMD Floating-Point Exception
extern void exception20();    // 0x14: Exception 20: Virtualization Exception
extern void exception21();    // 0x15: Exception 21: Control Protection Exception
extern void exception22();    // 0x16: Exception 22: Security Exception
extern void exception23();    // 0x17: Exception 23: Spare
extern void exception24();    // 0x18: Exception 24: Spare
extern void exception25();    // 0x19: Exception 25: Spare
extern void exception26();    // 0x1A: Exception 26: Spare
extern void exception27();    // 0x1B: Exception 27: Spare
extern void exception28();    // 0x1C: Exception 28: Spare
extern void exception29();    // 0x1D: Exception 29: Spare
extern void exception30();    // 0x1E: Exception 30: Spare
extern void exception31();    // 0x1F: Exception 31: Spare

// Pointer to the Interrupts in idt_helper.asm
extern void irq0();     // 0x20: IRQ 0: Programmable Interrupt Timer
extern void irq1();     // 0x21: IRQ 1: Keyboard
extern void irq2();     // 0x22: IRQ 2: Cascade (2)
extern void irq3();     // 0x23: IRQ 3: COM2 (If Enabled)
extern void irq4();     // 0x24: IRQ 4: COM1 (If Enabled)
extern void irq5();     // 0x25: IRQ 5: LPT2 (If Enabled)
extern void irq6();     // 0x26: IRQ 6: Floppy Disk
extern void irq7();     // 0x27: IRQ 7: LPT1/Spurious Interrupt
extern void irq8();     // 0x28: IRQ 8: CMOS Real-Time Clock
extern void irq9();     // 0x29: IRQ 9: Free for peripherals/legacy SCSI
extern void irq10();    // 0x2A: IRQ 10: Free for peripherals/SCSI/NIC
extern void irq11();    // 0x2B: IRQ 11: Free for peripherals/SCSI/NIC
extern void irq12();    // 0x2C: IRQ 12: PS/2 Mouse
extern void irq13();    // 0x2D: IRQ 13: FPU/Coprocessor/Inter-processor (Deprecated)
extern void irq14();    // 0x2E: IRQ 14: Primary ATA Hard Disk
extern void irq15();    // 0x2F: IRQ 15: Secondary ATA Hard Disk

extern void irq128();   // 0x80: IRQ 128: System Call


#endif