#include "procfs.h"

// . .. kernel pci registers

FILE* Procfs_root = NULL;

void procfs_open(FILE* node, uint32_t flags)
{
    return;
}

void procfs_close(FILE* node)
{
    return;
}

uint32_t procfs_read(FILE* node, uint32_t offset, uint32_t size, char* buffer)
{
    if(strcmp(node->name, "kernel") == 0)
    {
        if(offset == 0)
        {
            strcpy(buffer, "Kernel Name: ");
            strcat(buffer, KERNEL_NAME);
            strcat(buffer, "\n");
            strcat(buffer, "Kernel version: ");
            strcat(buffer, KERNEL_VERSION);
            strcat(buffer, "\n");
            strcat(buffer, "Kernel build date: ");
            strcat(buffer, KERNEL_BUILD);
            strcat(buffer, "\n");
            strcat(buffer, "Kernel build compiler: ");
            strcat(buffer, COMPILER);
            strcat(buffer, "\n");
        }
        return size;
    }

    if(strcmp(node->name, "pci") == 0)
    {
        procfs_enumerate_pci_devices(buffer);
        return size;
    }

    if(strcmp(node->name, "registers") == 0)
    {
        CPUSTATE_t* cpu = (CPUSTATE_t*)kmalloc(sizeof(CPUSTATE_t));
        asm volatile("mov %%eax, %0" : "=r" (cpu->eax));
        asm volatile("mov %%ebx, %0" : "=r" (cpu->ebx));
        asm volatile("mov %%ecx, %0" : "=r" (cpu->ecx));
        asm volatile("mov %%edx, %0" : "=r" (cpu->edx));
        asm volatile("mov %%esi, %0" : "=r" (cpu->esi));
        asm volatile("mov %%edi, %0" : "=r" (cpu->edi));
        asm volatile("mov %%ebp, %0" : "=r" (cpu->ebp));
        asm volatile("mov %%esp, %0" : "=r" (cpu->esp));

        strcpy(buffer, "Registers[May be Inaccurate]:\n");
        strcat(buffer, "eax: 0x");
        strcat(buffer, strndup("00000000", 8 - strlen(itoa_r(cpu->eax, 16))));
        strcat(buffer, itoa_r(cpu->eax, 16));
        strcat(buffer, "\n");
        strcat(buffer, "ebx: 0x");
        strcat(buffer, strndup("00000000", 8 - strlen(itoa_r(cpu->ebx, 16))));
        strcat(buffer, itoa_r(cpu->ebx, 16));
        strcat(buffer, "\n");
        strcat(buffer, "ecx: 0x");
        strcat(buffer, strndup("00000000", 8 - strlen(itoa_r(cpu->ecx, 16))));
        strcat(buffer, itoa_r(cpu->ecx, 16));
        strcat(buffer, "\n");
        strcat(buffer, "edx: 0x");
        strcat(buffer, strndup("00000000", 8 - strlen(itoa_r(cpu->edx, 16))));
        strcat(buffer, itoa_r(cpu->edx, 16));
        strcat(buffer, "\n");
        strcat(buffer, "esi: 0x");
        strcat(buffer, strndup("00000000", 8 - strlen(itoa_r(cpu->esi, 16))));
        strcat(buffer, itoa_r(cpu->esi, 16));
        strcat(buffer, "\n");
        strcat(buffer, "edi: 0x");
        strcat(buffer, strndup("00000000", 8 - strlen(itoa_r(cpu->edi, 16))));
        strcat(buffer, itoa_r(cpu->edi, 16));
        strcat(buffer, "\n");
        strcat(buffer, "ebp: 0x");
        strcat(buffer, strndup("00000000", 8 - strlen(itoa_r(cpu->ebp, 16))));
        strcat(buffer, itoa_r(cpu->ebp, 16));
        strcat(buffer, "\n");
        strcat(buffer, "esp: 0x");
        strcat(buffer, strndup("00000000", 8 - strlen(itoa_r(cpu->esp, 16))));
        strcat(buffer, itoa_r(cpu->esp, 16));
        strcat(buffer, "\n");
        strcat(buffer, "eip: 0x");
        strcat(buffer, strndup("00000000", 8 - strlen(itoa_r(cpu->eip, 16))));
        strcat(buffer, itoa_r(cpu->eip, 16));
        strcat(buffer, "\n\n");
        strcat(buffer, "eflags: 0x");
        strcat(buffer, strndup("00000000", 8 - strlen(itoa_r(cpu->eflags, 16))));
        strcat(buffer, itoa_r(cpu->eflags, 16));
        strcat(buffer, "\n");
        return size;
    }
}

char** procfs_list(FILE* node)
{
    if(node == Procfs_root)
    {
        char** list = (char**)kmalloc(sizeof(char*) * 3);
        list[0] = kmalloc(sizeof(char) * 5);
        list[1] = kmalloc(sizeof(char) * 5);
        list[2] = kmalloc(sizeof(char) * 10);
        list[3] = kmalloc(sizeof(char) * 10);

        strcpy(list[0], "kernel");
        strcpy(list[1], "pci");
        strcpy(list[2], "registers");

        return list;
    }
    return NULL;
}

vfs_node* procfs_finddir(vfs_node *parent, char *name)
{
    if(parent == Procfs_root)
    {
        if(strcmp(name, "kernel") == 0)
        {
            FILE* node = (FILE*)kmalloc(sizeof(FILE));
            strcpy(node->name, "kernel");
            node->flags = FS_FILE;
            node->open = procfs_open;
            node->close = procfs_close;
            node->read = procfs_read;
            return node;
        }
        if(strcmp(name, "pci") == 0)
        {
            FILE* node = (FILE*)kmalloc(sizeof(FILE));
            strcpy(node->name, "pci");
            node->flags = FS_DIRECTORY;
            node->open = procfs_open;
            node->close = procfs_close;
            node->read = procfs_read;
            return node;
        }
        if(strcmp(name, "registers") == 0)
        {
            FILE* node = (FILE*)kmalloc(sizeof(FILE));
            strcpy(node->name, "registers");
            node->flags = FS_FILE;
            node->open = procfs_open;
            node->close = procfs_close;
            node->read = procfs_read;
            return node;
        }
    }
    return NULL;
}

void init_procfs()
{
    Procfs_root = (FILE*)kmalloc(sizeof(FILE));
    strcpy(Procfs_root->name, "proc");
    Procfs_root->flags = FS_DIRECTORY;
    Procfs_root->open = procfs_open;
    Procfs_root->close = procfs_close;
    Procfs_root->listdir = procfs_list;
    Procfs_root->finddir = procfs_finddir;
}

void procfs_mount(char *path)
{
    VFS_mount(path, Procfs_root);
}