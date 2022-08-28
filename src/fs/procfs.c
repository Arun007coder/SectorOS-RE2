/**
 * Copyright (C) 2022 Arun007coder
 * 
 * This file is part of SectorOS-RE2.
 * 
 * SectorOS-RE2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * SectorOS-RE2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with SectorOS-RE2.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "procfs.h"

// . .. kernel pci registers

FILE* Procfs_root = NULL;

FILE procfs_nodes[16];
int current_node;

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

        strcpy(list[0], "kernel");
        strcpy(list[1], "pci");
        strcpy(list[2], "registers");

        list[3] = NULL;

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

    strcpy(procfs_nodes[0].name, "kernel");
    procfs_nodes[0].flags = FS_FILE;
    procfs_nodes[0].open = procfs_open;
    procfs_nodes[0].close = procfs_close;
    procfs_nodes[0].listdir = procfs_list;
    procfs_nodes[0].finddir = procfs_finddir;

    strcpy(procfs_nodes[1].name, "pci");
    procfs_nodes[1].flags = FS_FILE;
    procfs_nodes[1].open = procfs_open;
    procfs_nodes[1].close = procfs_close;
    procfs_nodes[1].listdir = procfs_list;
    procfs_nodes[1].finddir = procfs_finddir;

    strcpy(procfs_nodes[2].name, "registers");
    procfs_nodes[2].flags = FS_FILE;
    procfs_nodes[2].open = procfs_open;
    procfs_nodes[2].close = procfs_close;
    procfs_nodes[2].listdir = procfs_list;
    procfs_nodes[2].finddir = procfs_finddir;

    current_node = 3;

    for(int i = 0; i < current_node; i++)
    {
        char* buffer = (kmalloc(1023));
        strcpy(buffer, "/proc/");
        strcat(buffer, procfs_nodes[i].name);
        VFS_mount(buffer, &procfs_nodes[i]);
    }


}

void procfs_mount(char *path)
{
    VFS_mount(path, Procfs_root);
}