#ifndef __PROCFS_H__
#define __PROCFS_H__

#include "system.h"
#include "string.h"
#include "vfs.h"
#include "printf.h"
#include "kheap.h"
#include "pci.h"

void init_procfs();
void procfs_mount(char *path);

#endif