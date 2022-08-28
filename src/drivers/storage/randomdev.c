#include "randomdev.h"

uint32_t randomdev_read(vfs_node* node, uint32_t offset, uint32_t size, char* buffer)
{
    for(uint32_t i = 0; i < size; i++)
    {
        buffer[i] = rand() % 256;
    }
    return size;
}

uint32_t randomdev_write(vfs_node* node, uint32_t offset, uint32_t size, char* buffer)
{
    return size;
}

void randomdev_open(vfs_node* node, uint32_t offset)
{
    return;
}

void randomdev_close(vfs_node* node)
{
    return;
}

vfs_node* randomdev_getVFSNode()
{
    vfs_node* node = (vfs_node*)kmalloc(sizeof(vfs_node));
    memset(node, 0, sizeof(vfs_node));
    node->read = randomdev_read;
    node->write = randomdev_write;
    node->open = randomdev_open;
    node->close = randomdev_close;
    node->flags = FS_CHARDEVICE;
    node->size = 0;
    strcpy(node->name, "random");
    return node;
}

void init_randomdev()
{
    devfs_add(randomdev_getVFSNode());
    printl("randomdev initialized.\n");
}