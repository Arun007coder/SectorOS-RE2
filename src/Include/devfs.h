#ifndef __DEVFS_H_
#define __DEVFS_H_

#include "system.h"
#include "vfs.h"
#include "string.h"
#include "kheap.h"

#define MAX_DEVICES 64

void init_devfs();
void devfs_add(FILE* device);
void devfs_remove(char* name);
FILE* devfs_rnode();

void devfs_mount(char* mountpoint);

vfs_node *devfs_finddir(vfs_node *node, char *name);
DirectoryEntry* devfs_readdir(vfs_node *node, uint32_t index);
void devfs_open(vfs_node *node, uint32_t flags);
void devfs_close(vfs_node *node);
char** devfs_listdir(vfs_node* node);

#endif