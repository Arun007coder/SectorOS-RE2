#ifndef __ELF_H__
#define __ELF_H__

#warning ELF Support is not Implemented in the kernel.

#include "system.h"

#define DEFAULT_ELF_LOAD_ADDR (void*)0x08048000

#define ELF_NIDENT 16

#define ELF_MAG0 0x7f
#define ELF_MAG1 'E'
#define ELF_MAG2 'L'
#define ELF_MAG3 'F'

#define ELFDATA2LSB (1)
#define ELFCLASS32  (1)

#define EM_386 (3)
#define EV_CURRENT (1)

#define SHN_UNDEF  (0x00)

#define PT_LOAD    1
#define PT_DYNAMIC 2
#define PT_INTERP  3
#define PT_NOTE    4
#define PT_SHLIB   5
#define PT_PHDR    6
#define PT_LOPROC  0x70000000
#define PT_HIPROC  0x7fffffff

#define PF_X 1
#define PF_W 2
#define PF_R 4

typedef uint16_t elf32_half;
typedef uint32_t elf32_off;
typedef uint32_t elf32_addr;
typedef uint32_t elf32_word;

typedef int elf32_sword;

enum elf_ident
{
    EI_MAG0     = 0,
    EI_MAG1     = 1,
    EI_MAG2     = 2,
    EI_MAG3     = 3,
    EI_CLASS    = 4,
    EI_DATA     = 5,
    EI_VERSION  = 6,
    EI_OSABI    = 7,
    EI_ABIVERSION   = 8,
    EI_PAD      = 9
};

enum elf_type
{
    ET_NONE     = 0,
    ET_REL      = 1,
    ET_EXEC     = 2
};

enum sht_type
{
    SHT_NULL    = 0,
    SHT_PROGBITS    = 1,
    SHT_SYMTAB  = 2,
    SHT_STRTAB  = 3,
    SHT_RELA    = 4,
    SHT_NOBITS  = 8,
    SHT_REL     = 9,
};

enum sht_attr
{
    SHF_WRITE   = 1,
    SHF_ALLOC   = 2
};

typedef struct elf_header
{
    uint8_t     e_ident[ELF_NIDENT];
    elf32_half  e_type;
    elf32_half  e_machine;
    elf32_word  e_version;
    elf32_addr  e_entry;
    elf32_off   e_phoff;
    elf32_off   e_shoff;
    elf32_word  e_flags;
    elf32_half  e_ehsize;
    elf32_half  e_phentsize;
    elf32_half  e_phnum;
    elf32_half  e_shentsize;
    elf32_half  e_shnum;
    elf32_half  e_shstrndx;
} elf_header_t;

typedef struct elf_section_header
{
    elf32_word  sh_name;
    elf32_word  sh_type;
    elf32_word  sh_flags;
    elf32_addr  sh_addr;
    elf32_off   sh_offset;
    elf32_word  sh_size;
    elf32_word  sh_link;
    elf32_word  sh_info;
    elf32_word  sh_addralign;
    elf32_word  sh_entsize;
}elf_section_header_t;

typedef struct elf_program_header
{
    elf32_word      p_type;
    elf32_off       p_offset;
    elf32_addr      p_vaddr;
    elf32_addr      p_paddr;
    elf32_word      p_filesz;
    elf32_word      p_memsz;
    elf32_word      p_flags;
    elf32_word      p_align;
}elf_program_header_t;

#define ELF_SUCCESS             0x00
#define ELF_ERR_INVALID_MAGIC   0x01
#define ELF_ERR_INVALID_CLASS   0x02
#define ELF_ERR_INVALID_DATA    0x03
#define ELF_ERR_INVALID_VERSION 0x04
#define ELF_ERR_INVALID_ABI     0x05
#define ELF_ERR_INVALID_TYPE    0x06
#define ELF_ERR_INVALID_ARCH    0x07
#define ELF_ERR_INVALID_FILE    0x08
#define ELF_ERR_INVALID_HEADER  0x09
#define ELF_ERR_INVALID_SECTION 0x0a
#define ELF_ERR_INVALID_PROGRAM 0x0b

uint32_t isValidElf(elf_header_t *elf);
void load_elf();

#endif