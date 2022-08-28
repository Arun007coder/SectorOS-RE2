#include "nulldev.h"



uint32_t nulldev_read(vfs_node* node, uint32_t offset, uint32_t size, char* buffer)
{
    memset(buffer, 0, size);
    return size;
}

uint32_t nulldev_write(vfs_node* node, uint32_t offset, uint32_t size, char* buffer)
{
    return size;
}

void nulldev_open(vfs_node* node, uint32_t offset)
{
    return;
}

void nulldev_close(vfs_node* node)
{
    return;
}

int nulldev_get_size(vfs_node* node)
{
    return node->size;
}

vfs_node* nulldev_getVFSNode()
{
    vfs_node* node = (vfs_node*)kmalloc(sizeof(vfs_node));
    memset(node, 0, sizeof(vfs_node));
    node->read = nulldev_read;
    node->write = nulldev_write;
    node->open = nulldev_open;
    node->close = nulldev_close;
    node->get_size = nulldev_get_size;
    node->flags = FS_CHARDEVICE;
    node->size = 0;
    strcpy(node->name, "null");
    return node;
}

void init_nulldev()
{
    devfs_add(nulldev_getVFSNode());
    printl("nulldev initialized.\n");
}