#ifndef __NULLDEV_H__
#define __NULLDEV_H__

#include "system.h"
#include "vfs.h"
#include "kheap.h"
#include "string.h"
#include "devfs.h"
#include "logdisk.h"

#define NULLDEV_CHAR 0x00

void init_nulldev();

uint32_t nulldev_read(vfs_node *node, uint32_t offset, uint32_t size, char *buffer);
uint32_t nulldev_write(vfs_node *node, uint32_t offset, uint32_t size, char *buffer);

void nulldev_open(vfs_node *node, uint32_t flags);
void nulldev_close(vfs_node *node);

int nulldev_getFileSize(vfs_node *node);

vfs_node* nulldev_getVFSNode();

#endif