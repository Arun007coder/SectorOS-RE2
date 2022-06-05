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
    rnode->read = NULL;
    rnode->write = NULL;
    rnode->chmod = NULL;
    
    return rnode;
}

void devfs_init()
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

    nodes[num_nodes] = *node;
    num_nodes++;
    char* mountpoint = (char*)kcalloc(sizeof(char), 128);
    memset(mountpoint, 0, 128);
    strcpy(mountpoint, "/dev/");
    strcat(mountpoint, "/");
    strcat(mountpoint, node->name);
    printf("Mounting on %s\n", mountpoint);

    VFS_mountDev(mountpoint, node);
    kfree(mountpoint);
}

void devfs_remove(char* name)
{
    for (int i = 0; i < num_nodes; i++)
    {
        if (strcmp(nodes[i].name, name) == 0)
        {
            nodes[i] = nodes[num_nodes - 1];
            num_nodes--;
            return;
        }
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