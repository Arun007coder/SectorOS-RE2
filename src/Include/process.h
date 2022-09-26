#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "system.h"
#include "paging.h"
#include "kheap.h"
#include "string.h"
#include "printf.h"
#include "pit.h"
#include "list.h"
#include "gentree.h"
#include "math.h"
#include "isr.h"
#include "pic.h"
#include "keyboard.h"

#define TASK_RUNNING            0
#define TASK_INTERRUPTIBLE      1
#define TASK_UNINTERRUPTIBLE    2
#define TASK_ZOMBIE             4
#define TASK_STOPPED            8
#define TASK_SWAPPING           16
#define TASK_EXCLUSIVE          32
#define TASK_CREATED            64
#define TASK_LOADING            128

#define TASK_TYPE_USER   0
#define TASK_TYPE_KERNEL 1

/*
* Process manager
* Uses cooperative multitasking.
*/

typedef uint32_t pid_t;

typedef struct context
{
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t eflags;
    uint32_t cr3;
    uint32_t eip;
}__attribute__((packed)) context_t;

typedef void (*process_kbhandler_t)(char, int, int, uint8_t);

typedef struct process_control_block
{
    char filename[512];
    context_t regs;
    pid_t pid;
    listnode_t * self;
    void * stack;
    uint32_t state;
    uint32_t time_slice;
    page_directory_t * page_dir;

    uint32_t type;
    uint32_t entrypoint;

    uint32_t saved_address;
    process_kbhandler_t handler;
}pcb_t;

extern list_t* process_list;
extern pcb_t* current_process;
extern registers_t saved_context;

void init_processManager();

void change_process(pid_t pid);
void exit(uint32_t ret);

pid_t alloc_pid();
void create_process_from_routine(char* name, void* entrypoint, uint32_t type);

void list_process();

void user_regs_switch(context_t *regs2);
void kernel_regs_switch(context_t * regs2);

void process_kbh(uint8_t scancode);
void attach_handler(process_kbhandler_t handler);

#endif