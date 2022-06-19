#include "logdisk.h"

#define LOGFILE_PATH "/KernelLog.txt"

typedef struct logdisk_dev
{
    uint32_t size;
    uint32_t pos;
    char* buffer;
}logdisk_dev_t;


logdisk_dev_t logdisk;

void logdisk_store()
{
    vfs_node* node = file_open(LOGFILE_PATH, OPEN_WRONLY);
    if(node == NULL)
    {
        VFS_create(LOGFILE_PATH, 0);
        node = file_open(LOGFILE_PATH, OPEN_WRONLY);
    }
    VFS_write(node, 0, logdisk.size, logdisk.buffer);
}

void init_logdisk(uint32_t size)
{
    logdisk.size = size;
    logdisk.buffer = (char*)kmalloc(size);
}

void logdisk_mount()
{
    devfs_add(logdisk_getVFSNode());
}

void logdisk_chsize(uint32_t size)
{
    logdisk.size = size;
    char* new_buffer = (char*)kmalloc(size);
    memcpy(new_buffer, logdisk.buffer, logdisk.size);
    kfree(logdisk.buffer);
    logdisk.buffer = new_buffer;
}

uint32_t logdisk_read(FILE* file, uint32_t offset, uint32_t size, char* buf)
{
    if(offset + size > logdisk.size)
        size = logdisk.size - offset;
    for(uint32_t i = 0; i < size; i++)
        buf[i] = logdisk.buffer[offset + i];
    return size;
}

uint32_t logdisk_getfilesize(FILE* file)
{
    logdisk_dev_t* dev = (logdisk_dev_t*)file->device;
    return dev->size;
}

uint32_t logdisk_write(FILE* file, uint32_t offset, uint32_t size, char* buf)
{
    if(offset + size > logdisk.size)
        size = logdisk.size - offset;
    memcpy(logdisk.buffer + offset, buf, size);
    return size;
}

void logdisk_open(FILE* file, uint32_t flags)
{
    return;
}

void logdisk_close(FILE* file)
{
    return;
}

FILE* logdisk_getVFSNode()
{
    FILE* file = (FILE*)kcalloc(sizeof(FILE), 1);
    strcpy(file->name, "log");
    file->device = (void*)&logdisk;
    file->read = logdisk_read;
    file->write = logdisk_write;
    file->open = logdisk_open;
    file->close = logdisk_close;
    file->get_filesize = logdisk_getfilesize;
    file->size = logdisk.size;
    file->flags = FS_BLOCKDEVICE;
    return file;
}

void logdisk_putc(char c)
{
    logdisk.buffer[logdisk.pos] = c;
    logdisk.pos++;
    if(logdisk.pos >= logdisk.size)
        logdisk.pos = 0;
}

void logdisk_puts(char* str)
{
    while(*str != '\0')
    {
        logdisk_putc(*str);
        str++;
    }
}

char logdisk_getc(uint32_t offset)
{
    if(offset >= logdisk.size)
        return '\0';
    return logdisk.buffer[offset];
}

void logdisk_gets(uint32_t offset, char* buf, uint32_t size)
{
    if(offset + size > logdisk.size)
        size = logdisk.size - offset;
    memcpy(buf, logdisk.buffer + offset, size);
}

void printl(char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsprintf(NULL, logdisk_putc, fmt, args);
    va_end(args);
}

void printlog()
{
    uint32_t i = 0;
    while(logdisk.buffer[i] != '\0')
    {
        putchar(logdisk.buffer[i]);
        i++;
    }
}