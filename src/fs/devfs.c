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

#include "devfs.h"

FILE* root_node;
FILE nodes[MAX_DEVICES];
int num_nodes = 0;

FILE* devfs_getrootnode()
{
    FILE* rnode = (FILE*)kmalloc(sizeof(FILE));
    strcpy(rnode->name, "devfs");
    rnode->device = NULL;
    rnode->inode_num = 0;
    rnode->ref_count = 0;
    rnode->flags = FS_DIRECTORY;
    rnode->readdir = devfs_readdir;
    rnode->finddir = devfs_finddir;
    rnode->listdir = devfs_listdir;
    rnode->open = devfs_open;
    rnode->close = devfs_close;
    rnode->unlink = devfs_unlink;
    rnode->read = NULL;
    rnode->write = NULL;
    rnode->chmod = NULL;
    
    return rnode;
}

bool isAdded(FILE* node)
{
    for(int i = 0; i < num_nodes; i++)
    {
        if(&(nodes[i]) == node)
            return true;
    }
    return false;
}

void init_devfs()
{
    memset(nodes, 0, sizeof(FILE) * MAX_DEVICES);
    root_node = devfs_getrootnode();
    if(root_node == NULL)
    {
        printf("Could not create root node\n");
        return;
    }

    VFS_mountDev("/dev/", root_node);
}

void devfs_add(FILE* node)
{
    if (num_nodes >= MAX_DEVICES)
        return;

    if(node == NULL)
        return;

    if(isAdded(node))
        return;

    nodes[num_nodes] = *node;
    num_nodes++;
    char* mountpoint = (char*)kcalloc(sizeof(char), 128);
    memset(mountpoint, 0, 128);
    strcpy(mountpoint, "/dev");
    strcat(mountpoint, "/");
    strcat(mountpoint, node->name);
    printf("Mounting on %s\n", mountpoint);

    VFS_mountDev(mountpoint, node);
    kfree(mountpoint);
}

void devfs_unlink(FILE* parent, char* name)
{
    if(parent == NULL)
        return;
    if(name == NULL)
        return;
    if(parent == root_node)
    {
        devfs_remove(name);
    }
}

void devfs_remove(char* name)
{
    int i = 0;
    for (i = 0; i < num_nodes; i++)
    {
        if (strcmp(nodes[i].name, name) == 0)
        {
            if(nodes[i].unlink != NULL)
                nodes[i].unlink(nodes[i].device, NULL);
            memset(&nodes[i], 0, sizeof(FILE));
            return;
        }
    }

    for (int j = i; j < num_nodes; j++)
    {
        nodes[j] = nodes[j + 1];
    }
}

vfs_node *devfs_finddir(vfs_node *node, char *name)
{
    FILE* nnod;
    if(node == NULL || node != root_node)
        return NULL;

    for (int i = 0; i < num_nodes; i++)
    {
        if (strcmp(nodes[i].name, name) == 0)
        {
            nnod = &nodes[i];
            break;
        }
    }

    return nnod;
}

DirectoryEntry* devfs_readdir(vfs_node *node, uint32_t index)
{
    if(node == NULL || node != root_node)
        return NULL;

    if (index >= num_nodes)
        return NULL;

    DirectoryEntry* entry = (DirectoryEntry*)kmalloc(sizeof(DirectoryEntry));
    strcpy(entry->name, nodes[index].name);
    entry->inode_count = nodes[index].inode_num;
    return entry;
}

void devfs_open(vfs_node *node, uint32_t flags)
{
}

void devfs_close(vfs_node *node)
{
}

char** devfs_listdir(vfs_node* node)
{
    if(node == NULL || node != root_node)
        return NULL;

    char** list = (char**)kcalloc(sizeof(char*), num_nodes);
    int numdev = 0;
    for (int i = 0; i < num_nodes; i++)
    {
        list[i] = (char*)kcalloc(sizeof(char), 64);
        strcpy(list[i], nodes[i].name);
        numdev++;
    }
    list[numdev] = NULL;
    return list;
}

FILE* devfs_rnode()
{
    return root_node;
}

void devfs_mount(char* mountpoint)
{
    VFS_mountDev(mountpoint, root_node);
}