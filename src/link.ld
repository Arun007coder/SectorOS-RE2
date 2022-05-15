OUTPUT_FORMAT("elf32-i386")
ENTRY(kernel_entry)

paddr = 0x100000;
off = 0xC0000000;
vaddr = off + paddr;

SECTIONS
{
    . = 0xC0100000;
    .text ALIGN(4K) : AT(ADDR(.text)-0xC0000000)
    {
        *(.multiboot_data)
        *(.text)
    }
    .rodata ALIGN (4K) : AT(ADDR(.rodata)-0xC0000000)
    {
        *(.rodata)
    }
    .bss ALIGN(4K) : AT(ADDR(.bss)-0xC0000000)
    {
        *(.bss)
        *(.initial_stack)
    }
    .data ALIGN(4K) : AT(ADDR(.data)-0xC0000000)
    {
        *(.data)
    }

    end = .;

    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.note.gnu.build-id)
    }
}