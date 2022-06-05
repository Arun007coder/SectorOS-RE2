#ifndef __PAGING_H_
#define __PAGING_H_

#include "system.h"
#include "isr.h"
#include "pmm.h"
#include "math.h"

#define PAGE_SIZE 4096

#define IS_ALIGN(addr) ((((uint32_t)(addr)) | 0xFFFFF000) == 0)
#define PAGE_ALIGN(addr) ((((uint32_t)(addr)) & 0xFFFFF000) + 0x1000)

#define PAGEDIR_INDEX(vaddr) (((uint32_t)vaddr) >> 22)
#define PAGETBL_INDEX(vaddr) ((((uint32_t)vaddr) >>12) & 0x3ff)
#define PAGEFRAME_INDEX(vaddr) (((uint32_t)vaddr) & 0xfff)

#define SET_PGBIT(cr0) (cr0 = cr0 | 0x80000000)
#define CLEAR_PSEBIT(cr4) (cr4 = cr4 & 0xffffffef)

typedef struct page_dir_entry
{
    uint32_t present        :1;
    uint32_t rw             :1;
    uint32_t user           :1;
    uint32_t write_through  :1;
    uint32_t cache_disable  :1;
    uint32_t accessed       :1;
    uint32_t dirty          :1;
    uint32_t page_size      :1;
    uint32_t global         :1;
    uint32_t avail          :3;
    uint32_t frame          :20;
}page_dir_entry_t;

typedef struct page_table_entry
{
    uint32_t present        :1;
    uint32_t rw             :1;
    uint32_t user           :1;
    uint32_t reserved       :2;
    uint32_t accessed       :1;
    uint32_t dirty          :1;
    uint32_t reserved2      :2;
    uint32_t avail          :3;
    uint32_t frame          :20;
}page_table_entry_t;

typedef struct page_table
{
    page_table_entry_t pages[1024];
}page_table_t;

typedef struct page_directory
{
    page_dir_entry_t tables[1024];
    page_table_t* ref_tables[1024];
}page_directory_t;

extern int paging_enabled;

extern page_directory_t* TEMP_PAGE_DIR;

extern uint8_t * pmm_bitmap;
extern uint32_t  pmm_bitmap_size;

extern page_directory_t* kernel_page_dir;
extern page_directory_t* current_directory;

void* virt2phys(page_directory_t* dir, void* vaddr);
void* d_kmalloc(uint32_t size, int align);

void alloc_region(page_directory_t * dir, uint32_t start_vaddr, uint32_t end_vaddr, int iden_map, int is_kernel, int is_writable);
void free_region(page_directory_t * dir, uint32_t start_vaddr, uint32_t end_vaddr, int free);

void alloc_page(page_directory_t * dir, uint32_t vaddr, uint32_t frame, int is_kernel, int is_writable);
void free_page(page_directory_t * dir, uint32_t vaddr, int free);

void init_paging();

void switch_page_dir(page_directory_t* dir, uint32_t paddr);

void enable_paging();

void* ksbrk(uint32_t size);

void copy_page_dir(page_directory_t* dest, page_directory_t* src);
page_table_t* copy_page_table(page_directory_t * src_page_dir, page_directory_t * dst_page_dir, uint32_t page_dir_idx, page_table_t * src);

void page_fault_handler(registers_t regs);

#endif