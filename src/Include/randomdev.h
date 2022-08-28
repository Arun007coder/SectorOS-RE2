#ifndef __RANDOMDEV_H__
#define __RANDOMDEV_H__

#include "system.h"
#include "vfs.h"
#include "kheap.h"
#include "string.h"
#include "math.h"
#include "devfs.h"
#include "logdisk.h"

void init_randomdev();

uint32_t randomdev_read(vfs_node* node, uint32_t offset, uint32_t size, char *buffer);
uint32_t randomdev_write(vfs_node* node, uint32_t offset, uint32_t size, char *buffer);

void randomdev_open(vfs_node* node, uint32_t flags);
void randomdev_close(vfs_node* node);

vfs_node* randomdev_getVFSNode();

#endif