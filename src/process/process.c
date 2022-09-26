#include "process.h"

list_t *process_list;
pcb_t *current_process;
pcb_t *last_process;

uint32_t prev_jiffies;
pid_t curr_pid;
registers_t saved_context;

pid_t alloc_pid()
{
    return curr_pid++;
}

void context_switch(registers_t *p_regs, context_t *n_regs)
{
    if (last_process)
    {
        last_process->regs.eax = p_regs->eax;
        last_process->regs.ebx = p_regs->ebx;
        last_process->regs.ecx = p_regs->ecx;
        last_process->regs.edx = p_regs->edx;
        last_process->regs.esi = p_regs->esi;
        last_process->regs.edi = p_regs->edi;
        last_process->regs.ebp = p_regs->ebp;
        last_process->regs.esp = p_regs->useresp;
        last_process->regs.eflags = p_regs->eflags;
        last_process->regs.eip = p_regs->eip;
        asm volatile("mov %%cr3, %0" : "=r"(last_process->regs.cr3));
    }

    if (((page_directory_t *)n_regs->cr3) != NULL)
    {
        switch_page_dir((page_directory_t *)n_regs->cr3, 1);
    }

    pic_eoi(0);
    last_process = current_process;

    if (current_process->type == TASK_TYPE_KERNEL)
    {
        kernel_regs_switch(n_regs);
    }
    else if (current_process->type == TASK_TYPE_USER)
    {
        user_regs_switch(n_regs);
    }
}

pcb_t* get_process_by_pid(pid_t pid)
{
    foreach(t, process_list)
    {
        pcb_t *pcb = (pcb_t *)t->val;

        if(pcb->pid == pid)
        {
            return pcb;
        }
    }
    return NULL;
}

void change_process(pid_t pid)
{
    pcb_t *pcb = get_process_by_pid(pid);

    if(pid == 0)
    {
        if(pcb == NULL)
        {
            kernel_panic(" No process left! Never exit userspace init process...\n ");
        }
    }

    while(pcb == NULL)
    {
        pcb = get_process_by_pid(pid--);
    }

    if(pcb->state == TASK_STOPPED)
    {
        pcb = get_process_by_pid((pid == 0 ? 0 : (pid--)));
    }

    current_process = pcb;

    context_switch(&saved_context, &(pcb->regs));
}

void exit(uint32_t ret)
{
    pid_t pid = current_process->pid;

    printf("[ PMGR ] Process %d exited with code %d.\n", pid, ret);
    serial_printf("[ PMGR ] Process %d exited with code %d.\n", pid, ret);

    current_process->state = TASK_STOPPED;

    if(current_process->page_dir != NULL)
    {
        kfree(current_process->page_dir);
    }

    list_remove_node(process_list, current_process->self);

    kfree(current_process);

    last_process = NULL;

    change_process((pid == 0 ? 0 : (pid--)));
}

void create_process_from_routine(char *name, void *entrypoint, uint32_t type)
{
    pcb_t *pcb = kcalloc(sizeof(pcb_t), 1);

    strcpy(pcb->filename, name);
    pcb->pid = alloc_pid();

    pcb->entrypoint = (uint32_t)entrypoint;

    pcb->regs.esp = 0xC0000000;
    pcb->regs.eflags = 0x206;
    pcb->regs.eip = pcb->entrypoint;

    pcb->page_dir = kmalloc_a(sizeof(page_directory_t));
    memset(pcb->page_dir, 0, sizeof(page_directory_t));

    copy_page_dir(pcb->page_dir, kernel_page_dir);
    alloc_region(pcb->page_dir, 0xC0000000 - 4 * PAGE_SIZE, 0xC0000000, 0, 0, 1);
    pcb->regs.cr3 = (uint32_t)virt2phys(kernel_page_dir, pcb->page_dir);

    pcb->type = type;
    pcb->state = TASK_CREATED;

    pcb->handler = 0;

    pcb->self = list_insert_front(process_list, pcb);

    printf("[ PMGR ] Created process %d: %s.\n", pcb->pid, pcb->filename);
    serial_printf("[ PMGR ] Created process %d: %s.\n", pcb->pid, pcb->filename);

    change_process(pcb->pid);
}

void attach_handler(process_kbhandler_t handler)
{
    current_process->handler = handler;
}

void init_processManager()
{
    process_list = list_create();
    init_keyboard(process_kbh);
    printf("[ PMGR ] Process manager initialized...\n");
}

void list_process()
{
    printf("No. of processes: %d\n", curr_pid);
    foreach (t, process_list)
    {
        pcb_t *p = t->val;
        printf("%s%d : Name: %s, Type: %s, State: %s\n", (current_process->pid == p->pid ? ">" : " "), p->pid, p->filename, (p->type == TASK_TYPE_KERNEL ? "Kernel task" : "Userspace task"), (p->state == TASK_CREATED ? "Created" : (p->state == TASK_RUNNING ? "Running" : (p->state == TASK_LOADING ? "Loading" : "Unknown"))));
    }
}

int isShift = 0;

void process_kbh(uint8_t scancode)
{
    char key;
    int isCTRL = 0;
    int isALT = 0;

    int isCHPMode;
    char CHP_L[32];

    int i;

    pid_t CHP_PID;

    if (scancode < 80)
    {
        switch (scancode)
        {
        case 0xFA:
            break;
        case 0x45:
        case 0xC5:
            break;
        case 0x1D:
            isCTRL = 1;
            break;
        case 0x38:
            isALT = 1;
            break;
        case 0x2A:
        case 0x36:
            isShift = 1;
            break;
        case 0x0E:
            key = '\b';
            break;
        case 0x0F:
            key = '\t';
            break;
        case 0x29:
            if (!isShift)
                key = '`';
            else
                key = '~';
            break;
        case 0x02:
            if (!isShift)
                key = '1';
            else
                key = '!';
            break;
        case 0x03:
            if (!isShift)
                key = '2';
            else
                key = '@';
            break;
        case 0x04:
            if (!isShift)
                key = '3';
            else
                key = '#';
            break;
        case 0x05:
            if (!isShift)
                key = '4';
            else
                key = '$';
            break;
        case 0x06:
            if (!isShift)
                key = '5';
            else
                key = '%';
            break;
        case 0x07:
            if (!isShift)
                key = '6';
            else
                key = '^';
            break;
        case 0x08:
            if (!isShift)
                key = '7';
            else
                key = '&';
            break;
        case 0x09:
            if (!isShift)
                key = '8';
            else
                key = '*';
            break;
        case 0x0A:
            if (!isShift)
                key = '9';
            else
                key = '(';
            break;
        case 0x0B:
            if (!isShift)
                key = '0';
            else
                key = ')';
            break;
        case 0x0C:
            if (!isShift)
                key = '-';
            else
                key = '_';
            break;
        case 0x0D:
            if (!isShift)
                key = '=';
            else
                key = '+';
            break;

        case 0x10:
            if (!isShift)
                key = 'q';
            else
                key = 'Q';
            break;
        case 0x11:
            if (!isShift)
                key = 'w';
            else
                key = 'W';
            break;
        case 0x12:
            if (!isShift)
                key = 'e';
            else
                key = 'E';
            break;
        case 0x13:
            if (!isShift)
                key = 'r';
            else
                key = 'R';
            break;
        case 0x14:
            if (!isShift)
                key = 't';
            else
                key = 'T';
            break;
        case 0x15:
            if (!isShift)
                key = 'y';
            else
                key = 'Y';
            break;
        case 0x16:
            if (!isShift)
                key = 'u';
            else
                key = 'U';
            break;
        case 0x17:
            if (!isShift)
                key = 'i';
            else
                key = 'I';
            break;
        case 0x18:
            if (!isShift)
                key = 'o';
            else
                key = 'O';
            break;
        case 0x19:
            if (!isShift)
                key = 'p';
            else
                key = 'P';
            break;
        case 0x1A:
            if (!isShift)
                key = '[';
            else
                key = '{';
            break;
        case 0x1B:
            if (!isShift)
                key = ']';
            else
                key = '}';
            break;
        case 0x1C:
            if (!isShift)
                key = '\n';
            else
                key = '\n';
            break;

        case 0x1E:
            if (!isShift)
                key = 'a';
            else
                key = 'A';
            break;
        case 0x1F:
            if (!isShift)
                key = 's';
            else
                key = 'S';
            break;
        case 0x20:
            if (!isShift)
                key = 'd';
            else
                key = 'D';
            break;
        case 0x21:
            if (!isShift)
                key = 'f';
            else
                key = 'F';
            break;
        case 0x22:
            if (!isShift)
                key = 'g';
            else
                key = 'G';
            break;
        case 0x23:
            if (!isShift)
                key = 'h';
            else
                key = 'H';
            break;
        case 0x24:
            if (!isShift)
                key = 'j';
            else
                key = 'J';
            break;
        case 0x25:
            if (!isShift)
                key = 'k';
            else
                key = 'K';
            break;
        case 0x26:
            if (!isShift)
                key = 'l';
            else
                key = 'L';
            break;
        case 0x27:
            if (!isShift)
                key = ';';
            else
                key = ':';
            break;

        case 0x2C:
            if (!isShift)
                key = 'z';
            else
                key = 'Z';
            break;
        case 0x2D:
            if (!isShift)
                key = 'x';
            else
                key = 'X';
            break;
        case 0x2E:
            if (!isShift)
                key = 'c';
            else
                key = 'C';
            break;
        case 0x2F:
            if (!isShift)
                key = 'v';
            else
                key = 'V';
            break;
        case 0x30:
            if (!isShift)
                key = 'b';
            else
                key = 'B';
            break;
        case 0x31:
            if (!isShift)
                key = 'n';
            else
                key = 'N';
            break;
        case 0x32:
            if (!isShift)
                key = 'm';
            else
                key = 'M';
            break;
        case 0x33:
            if (!isShift)
                key = ',';
            else
                key = '<';
            break;
        case 0x34:
            if (!isShift)
                key = '.';
            else
                key = '>';
            break;
        case 0x35:
            if (!isShift)
                key = '/';
            else
                key = '?';
            break;
        case 0x39:
            if (!isShift)
                key = ' ';
            else
                key = ' ';
            break;
        case 0x2B:
            if (!isShift)
                key = '\\';
            else
                key = '|';
            break;
        case 0x3A:
            if (isShift)
            {
                isShift = false;
            }
            else
            {
                isShift = true;
            }
            break;
        default:
            key = 0;
            break;
        }

        if (isCHPMode == 1)
        {
            if (key != '\n')
            {
                printf(key);
                CHP_L[i] = key;
                i++;
            }
            else
            {
                CHP_L[i] == '\0';
                CHP_PID = atoi(CHP_L);
                isCHPMode = 0;
                memset(CHP_L, 0, 32);
                printf("\n");
                change_process(CHP_PID);
            }
        }
    }
    else if (scancode == 0xAA || scancode == 0xB6)
    {
        isShift = false;
    }

    process_kbhandler_t handler = current_process->handler;

    if (handler == 0)
    {
        return;
    }

    if(isCHPMode != 1)
    {
            handler(key, isCTRL, isALT, scancode);
    }
}
