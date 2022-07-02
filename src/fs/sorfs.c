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

#include "sorfs.h"

vfs_node* sorfs_root;

void sorfs_readBlock(sorfs_t* fs, uint32_t blockNum, sorfs_block_t* block)
{
    VFS_read(fs->device, sizeof(sorfs_superblock_t) + (blockNum * sizeof(sorfs_block_t)), sizeof(sorfs_block_t), (char*)block);

    if(block->magic != SORFS_BLOCK_MAGIC)
    {
        printf("[SORFS] Block %d is corrupted\n", blockNum);
        return;
    }
}

void sorfs_readSB(sorfs_t* fs)
{
    VFS_read(fs->device, 0, sizeof(sorfs_superblock_t), (char*)fs->sb);
}

void sorfs_readBlockContent(sorfs_t* fs, sorfs_block_t* block, uint32_t offset, uint32_t size, char *buffer)
{
    VFS_read(fs->device, block->offset + offset, size, buffer);
}

uint32_t sorfs_getFileSize(FILE* f)
{
    sorfs_t* fs = (sorfs_t*)f->device;
    sorfs_block_t* block = kmalloc(sizeof(sorfs_block_t));
    sorfs_readBlock(fs, f->inode_num, block);
    uint32_t size = block->size;
    kfree(block);
    return size;
}

FILE* sorfs_BlockToVFSNode(sorfs_t* fs, uint32_t blockNum)
{
    if(blockNum >= fs->sb->total_blocks)
    {
        printf("[SORFS] Block %d is out of bounds\n", blockNum);
        return NULL;
    }

    FILE* f = kmalloc(sizeof(FILE));
    sorfs_block_t* block = kmalloc(sizeof(sorfs_block_t));
    sorfs_readBlock(fs, blockNum, block);

    if(block->type == SORFS_TYPE_FRE)
    {
        printf("[SORFS] Block %d is free\n", blockNum);
        kfree(block);
        kfree(f);
        return NULL;
    }

    strcpy(f->name, block->name);
    f->device = fs;
    f->open = sorfs_open;
    f->close = sorfs_close;
    f->size = block->size;
    f->inode_num = blockNum;

    if(block->type == SORFS_TYPE_DIR)
    {
        f->finddir = sorfs_finddir;
    }

    if(block->type == SORFS_TYPE_REG)
    {
        f->read = sorfs_read;
        f->write = sorfs_write;
        f->get_filesize = sorfs_getFileSize;
    }
    
    return f;
}

uint32_t sorfs_read(FILE* f, uint32_t offset, uint32_t size, char* buffer)
{
    sorfs_t* fs = (sorfs_t*)f->device;
    sorfs_block_t* block = kmalloc(sizeof(sorfs_block_t));
    sorfs_readBlock(fs, f->inode_num, block);
    sorfs_readBlockContent(fs, block, offset, size, buffer);
    kfree(block);
    return size;
}

uint32_t sorfs_write(FILE* f, uint32_t offset, uint32_t size, char* buffer)
{
    printf("[SORFS] Cannot write to a read-only filesystem\n");
}

char** sorfs_listdir(FILE* node)
{
    sorfs_t* fs = (sorfs_t*)node->device;

    if(node == sorfs_root)
    {
        char** list = kmalloc(sizeof(char*) * (fs->sb->total_blocks - fs->free_blocks) + 1);

        int i;
        for(i = 0; i < (fs->total_blocks - fs->free_blocks); i++)
        {
            FILE* f = sorfs_BlockToVFSNode(fs, i);
            if(f == NULL)
                continue;
            list[i] = kmalloc(sizeof(char) * 256);
            strcpy(list[i], f->name);
        }
        list[i] = NULL;
        return list;
    }
}

FILE* sorfs_finddir(FILE* node, char* name)
{
    sorfs_t* fs = (sorfs_t*)node->device;
    if(node == sorfs_root)
    {
        for(int i = 0; i < (fs->total_blocks - fs->free_blocks); i++)
        {
            FILE* f = sorfs_BlockToVFSNode(fs, i);
            if(f == NULL)
                continue;
            if(strcmp(f->name, name) == 0)
                return f;
        }
    }
}

void sorfs_open(FILE* f, uint32_t flags)
{
    return;
}

void sorfs_close(FILE* f)
{
    return;
}

int isSORFS(char* devpath)
{
    sorfs_t* fs = (sorfs_t*)kmalloc(sizeof(sorfs_t));
    fs->device = file_open(devpath, 0);
    fs->sb = kmalloc(sizeof(sorfs_superblock_t));
    sorfs_readSB(fs);
    if(fs->sb->magic != SORFS_SB_MAGIC)
    {
        VFS_close(fs->device);
        kfree(fs);
        return 0;
    }
    VFS_close(fs->device);
    return 1;
}

void init_sorfs(char* dev_path, char* mountpath)
{
    sorfs_t* fs = kmalloc(sizeof(sorfs_t));
    fs->device = file_open(dev_path, OPEN_RDWR);
    fs->sb = kmalloc(sizeof(sorfs_superblock_t));
    sorfs_readSB(fs);

    if(fs->sb->magic != SORFS_SB_MAGIC)
    {
        printf("[SORFS] Not a valid SORFS filesystem\n");
        return;
    }

    fs->free_blocks = fs->sb->free_blocks;
    fs->total_blocks = fs->sb->total_blocks;

    printl("[SORFS] Mounting %s at %s\n", dev_path, mountpath);

    sorfs_root = kmalloc(sizeof(vfs_node));
    memset(sorfs_root, 0, sizeof(vfs_node));

    strcpy(sorfs_root->name, mountpath);
    sorfs_root->device = fs;
    sorfs_root->listdir = sorfs_listdir;
    sorfs_root->finddir = sorfs_finddir;
    sorfs_root->read = sorfs_read;
    sorfs_root->write = sorfs_write;
    sorfs_root->open = sorfs_open;
    sorfs_root->close = sorfs_close;
    sorfs_root->get_filesize = sorfs_getFileSize;
    sorfs_root->size = fs->sb->size;

    sorfs_root->flags = FS_DIRECTORY;

    VFS_mount(mountpath, sorfs_root);

    printl("[SORFS] Mounted %s at %s\n", dev_path, mountpath);

    return;
}