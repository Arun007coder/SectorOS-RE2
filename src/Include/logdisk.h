#ifndef __LOGDISK_H__
#define __LOGDISK_H__

#include "system.h"
#include "vfs.h"
#include "devfs.h"
#include "kheap.h"
#include "printf.h"
#include "string.h"

void init_logdisk(uint32_t size);

void logdisk_chsize(uint32_t new_size);

void logdisk_putc(char c);
void logdisk_puts(char *str);
void printl(char* fmt, ...);

char logdisk_getc(uint32_t offset);
void logdisk_gets(uint32_t offset, char* buf, uint32_t size);

void printlog();

void logdisk_mount();

FILE* logdisk_getVFSNode();
#endif