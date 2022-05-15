#include "paging.h"
#include "kheap.h"

extern void * heap_start, * heap_end, * heap_max, * heap_curr;
extern bool kheap_enabled;
bool kheap_enabled;

uint8_t * temp_mem;
int paging_enabled = 0;

page_directory_t * kpage_dir;


void* virt2phys(page_directory_t* dir, void* vaddr)
{
    if(!paging_enabled)
    {
        return (void*)(vaddr - KERNEL_BASE);
    }
    uint32_t page_dir_idx = PAGEDIR_INDEX(vaddr), page_tbl_idx = PAGETBL_INDEX(vaddr), page_frame_offset = PAGEFRAME_INDEX(vaddr);
    if(!dir->ref_tables[page_dir_idx])
    {
        printE("virt2phys: Page directory entry not found!\n");
        return NULL;
    }
    page_table_t * table = dir->ref_tables[page_dir_idx];
    if(!table->pages[page_tbl_idx].present)
    {
        printE("virt2phys: Page table entry does not exist\n");
        return NULL;
    }
    uint32_t t = table->pages[page_tbl_idx].frame;
    t = (t << 12) + page_frame_offset;
    return (void*)t;
}

void* d_kmalloc(uint32_t size, int align)
{
    void * ret = temp_mem;
    if(align && !IS_ALIGN(ret))
        ret = (void*)PAGE_ALIGN(ret);
    temp_mem = temp_mem + size;
    return ret;
}

void alloc_region(page_directory_t * dir, uint32_t start_vaddr, uint32_t end_vaddr, int iden_map, int is_kernel, int is_writable)
{
    uint32_t start = start_vaddr & 0xfffff000;
    uint32_t end = end_vaddr & 0xfffff000;
    while(start <= end)
    {
        if(iden_map)
            alloc_page(dir, start, start / PAGE_SIZE, is_kernel, is_writable);
        else
            alloc_page(dir, start, 0, is_kernel, is_writable);
        start = start + PAGE_SIZE;
    }
}

void free_region(page_directory_t * dir, uint32_t start_vaddr, uint32_t end_vaddr, int free)
{
    uint32_t start = start_vaddr & 0xfffff000;
    uint32_t end = end_vaddr & 0xfffff000;
    while(start <= end)
    {
        free_page(dir, start, 1);
        start = start + PAGE_SIZE;
    }
}

void alloc_page(page_directory_t * dir, uint32_t vaddr, uint32_t frame, int is_kernel, int is_writable)
{
    page_table_t * table = NULL;
    if(!dir)
    {
        printE("alloc_page: page directory is empty\n");
        return;
    }
    
    uint32_t page_dir_idx = PAGEDIR_INDEX(vaddr), page_tbl_idx = PAGETBL_INDEX(vaddr);
    table = dir->ref_tables[page_dir_idx];
    if(!table)
    {
        if(!kheap_enabled)
            table = d_kmalloc(sizeof(page_table_t), 1);
        else
            table = kmalloc_a(sizeof(page_table_t));

        memset(table, 0, sizeof(page_table_t));
        uint32_t t = (uint32_t)virt2phys(kpage_dir, table);
        dir->tables[page_dir_idx].frame = t >> 12;
        dir->tables[page_dir_idx].present = 1;
        dir->tables[page_dir_idx].rw = 1;
        dir->tables[page_dir_idx].user = 1;
        dir->tables[page_dir_idx].page_size = 0;
        dir->ref_tables[page_dir_idx] = table;
    }

    if(!table->pages[page_tbl_idx].present) 
    {
        uint32_t t;
        if(frame)
            t = frame;
        else
            t = alloc_block();
        table->pages[page_tbl_idx].frame = t;
        table->pages[page_tbl_idx].present = 1;
        table->pages[page_tbl_idx].rw = 1;
        table->pages[page_tbl_idx].user = 1;
    }
}

void free_page(page_directory_t * dir, uint32_t vaddr, int free)
{
    if(dir == TEMP_PAGE_DIR)
        return;
    uint32_t page_dir_idx = PAGEDIR_INDEX(vaddr), page_tbl_idx = PAGETBL_INDEX(vaddr);
    if(!dir->ref_tables[page_dir_idx])
    {
        printE("free_page: page dir entry does not exist\n");
        return;
    }
    page_table_t * table = dir->ref_tables[page_dir_idx];
    if(!table->pages[page_tbl_idx].present)
    {
        printE("free_page: page table entry does not exist\n");
        return;
    }
    if(free)
        free_block(table->pages[page_tbl_idx].frame);
    table->pages[page_tbl_idx].present = 0;
    table->pages[page_tbl_idx].frame = 0;
}

void init_paging()
{
    temp_mem = pmm_bitmap + pmm_bitmap_size;

    kpage_dir = d_kmalloc(sizeof(page_directory_t), 1);
    memset(kpage_dir, 0, sizeof(page_directory_t));

    uint32_t i = KERNEL_BASE;
    while(i < KERNEL_BASE + 4 * MB)
    {
        alloc_page(kpage_dir, i, 0, 1, 1);
        i = i + PAGE_SIZE;
    }

    i = KERNEL_BASE + 4 * MB;
    while(i < KERNEL_BASE + 4 * MB + KHEAP_INITIAL_SIZE)
    {
        alloc_page(kpage_dir, i, 0, 1, 1);
        i = i + PAGE_SIZE;
    }

    register_interrupt_handler(14, page_fault_handler);

    switch_page_dir(kpage_dir, 0);

    enable_paging();

    alloc_region(kpage_dir, 0x0, 0x10000, 1, 1, 1);

    printf("Paging initialized\n");
}

void switch_page_dir(page_directory_t* dir, uint32_t paddr)
{
    uint32_t t;
    if(!paddr)
        t = (uint32_t)virt2phys(TEMP_PAGE_DIR, dir);
    else
        t = (uint32_t)dir;
    asm volatile("mov %0, %%cr3" :: "r"(t));
}

void enable_paging()
{
    uint32_t cr0, cr4;

    asm volatile("mov %%cr4, %0" : "=r"(cr4));
    CLEAR_PSEBIT(cr4);
    asm volatile("mov %0, %%cr4" :: "r"(cr4));

    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    SET_PGBIT(cr0);
    asm volatile("mov %0, %%cr0" :: "r"(cr0));

    paging_enabled = 1;
}

void* ksbrk(uint32_t size)
{
    void * runner = NULL;
    void * new_boundary;
    void * old_heap_curr;
restart_sbrk:
    old_heap_curr = heap_curr;
    if(size == 0)
    {
        goto ret;
    }
    else if(size > 0)
    {
        new_boundary = heap_curr + (uint32_t)size;
        if(new_boundary <= heap_end)
            goto update_boundary;
        else if(new_boundary > heap_max)
            return NULL;
        else if(new_boundary > heap_end) 
        {
            runner = heap_end;
            while(runner < new_boundary)
            {
                alloc_page(kpage_dir, (uint32_t)runner, 0, 1, 1);
                runner = runner +  PAGE_SIZE;
            }
            if(old_heap_curr != heap_curr)
            {
                goto restart_sbrk;
            }
            heap_end = runner;
            goto update_boundary;
        }
    }
    else if(size < 0)
    {
        new_boundary = (void*)((uint32_t)heap_curr - (uint32_t)abs(size));
        if(new_boundary < heap_start + HEAP_MIN_SIZE)
        {
            new_boundary = heap_start + HEAP_MIN_SIZE;
        }
        runner = heap_end - PAGE_SIZE;
        while(runner > new_boundary)
        {
            free_page(kpage_dir, (uint32_t)runner, 1);
            runner = runner - PAGE_SIZE;
        }
        heap_end = runner + PAGE_SIZE;
        goto update_boundary;
    }
update_boundary:
    heap_curr = new_boundary;
ret:
    return old_heap_curr;
}

void copy_page_dir(page_directory_t* dst, page_directory_t* src)
{
    for(uint32_t i = 0; i < 1024; i++) {
        if(kpage_dir->ref_tables[i] == src->ref_tables[i])
        {
            dst->tables[i] = src->tables[i];
            dst->ref_tables[i] = src->ref_tables[i];
        }
        else
        {
            dst->ref_tables[i] = copy_page_table(src, dst, i, src->ref_tables[i]);
            uint32_t phys = (uint32_t)virt2phys(src, dst->ref_tables[i]);
            dst->tables[i].frame = phys >> 12;
            dst->tables[i].user = 1;
            dst->tables[i].rw = 1;
            dst->tables[i].present = 1;
        }
    }
}

page_table_t* copy_page_table(page_directory_t * src_page_dir, page_directory_t * dst_page_dir, uint32_t page_dir_idx, page_table_t * src)
{
    page_table_t * table = (page_table_t*)kmalloc_a(sizeof(page_table_t));
    for(int i = 0; i < 1024; i++)
    {
        if(!table->pages[i].frame)
            continue;
        uint32_t src_virtual_address = (page_dir_idx << 22) | (i << 12) | (0);
        uint32_t dst_virtual_address = src_virtual_address;
        uint32_t tmp_virtual_address = 0;

        alloc_page(dst_page_dir, dst_virtual_address, 0, 0, 1);
        alloc_page(src_page_dir, tmp_virtual_address, (uint32_t)virt2phys(dst_page_dir, (void*)dst_virtual_address), 0, 1);
        if (src->pages[i].present) table->pages[i].present = 1;
        if (src->pages[i].rw)      table->pages[i].rw = 1;
        if (src->pages[i].user)    table->pages[i].user = 1;
        if (src->pages[i].accessed)table->pages[i].accessed = 1;
        if (src->pages[i].dirty)   table->pages[i].dirty = 1;
        memcpy((void*)tmp_virtual_address, (void*)src_virtual_address, PAGE_SIZE);
        free_page(src_page_dir, tmp_virtual_address, 0);
    }
    return table;
}

void page_fault_handler(registers_t regs)
{
    printE("Page fault:\n");
    uint32_t faulting_addr;
    asm volatile("mov %%cr2, %0" : "=r" (faulting_addr));
    uint32_t present = regs.ecode & 0x1;
    uint32_t rw = regs.ecode & 0x2;
    uint32_t user = regs.ecode & 0x4;
    uint32_t reserved = regs.ecode & 0x8;
    uint32_t inst_fetch = regs.ecode & 0x10;

    printE("Possible causes: [ ");
    if(!present) printE("Page not present ");
    if(rw) printE("Page is read only ");
    if(user) printE("Page is read only ");
    if(reserved) printE("Overwrote reserved bits ");
    if(inst_fetch) printE("Instruction fetch ");
    printE("]\n");

    printE("Faulting address: 0x%x\n", faulting_addr);
    PANIC("Page fault");
}