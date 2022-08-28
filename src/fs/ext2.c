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

#include "ext2.h"

uint32_t EXT2_getFileSize(vfs_node *node)
{
    ext2_fs *fs = (ext2_fs *)node->device;
    inode_t *inode = (inode_t *)kmalloc(sizeof(inode_t));
    read_inode_metadata(fs, inode, node->inode_num);
    uint32_t res = inode->size;
    kfree(inode);
    return res;
}

void EXT2_mkdir(vfs_node *parent, char *name, uint16_t permission)
{
    ext2_fs *fs = (ext2_fs *)parent->device;
    uint32_t inode_num = alloc_inode(fs);
    inode_t *inode = (inode_t *)kmalloc(sizeof(inode_t));
    read_inode_metadata(fs, inode, inode_num);
    inode->permission = EXT2_IFDIR;
    inode->permission |= 0xFFF & permission;
    inode->atime = 0;
    inode->ctime = 0;
    inode->dtime = 0;
    inode->gid = 0;
    inode->uid = 0;
    inode->f_block_addr = 0;
    inode->num_sectors = 0;
    inode->size = fs->block_size;
    inode->hard_links = 2;
    inode->flags = 0;
    inode->file_acl = 0;
    inode->dir_acl = 0;
    inode->generation = 0;
    inode->os_specific1 = 0;
    memset(inode->blocks, 0, sizeof(inode->blocks));
    memset(inode->os_specific2, 0, 12);

    alloc_IBlock(fs, inode, inode_num, 0);
    write_inode_metadata(fs, inode, inode_num);
    EXT2_createEntry(parent, name, inode_num);

    inode_t *p_inode = (inode_t *)kmalloc(sizeof(inode_t));
    read_inode_metadata(fs, p_inode, parent->inode_num);
    p_inode->hard_links++;
    write_inode_metadata(fs, p_inode, parent->inode_num);
    rewrite_bgd(fs);
}

void EXT2_mkfile(vfs_node *parent, char *name, uint16_t permission)
{
    ext2_fs *fs = (ext2_fs *)parent->device;
    uint32_t inode_num = alloc_inode(fs);
    inode_t *inode = (inode_t *)kmalloc(sizeof(inode_t));
    read_inode_metadata(fs, inode, inode_num);
    inode->permission = EXT2_IFREG;
    inode->permission |= 0xFFF & permission;
    inode->atime = 0;
    inode->ctime = 0;
    inode->dtime = 0;
    inode->gid = 0;
    inode->uid = 0;
    inode->f_block_addr = 0;
    inode->num_sectors = 0;
    inode->size = fs->block_size;
    inode->hard_links = 2;
    inode->flags = 0;
    inode->file_acl = 0;
    inode->dir_acl = 0;
    inode->generation = 0;
    inode->os_specific1 = 0;
    memset(inode->blocks, 0, sizeof(inode->blocks));
    memset(inode->os_specific2, 0, 12);

    alloc_IBlock(fs, inode, inode_num, 0);
    write_inode_metadata(fs, inode, inode_num);
    EXT2_createEntry(parent, name, inode_num);

    inode_t *p_inode = (inode_t *)kmalloc(sizeof(inode_t));
    read_inode_metadata(fs, p_inode, parent->inode_num);
    p_inode->hard_links++;
    write_inode_metadata(fs, p_inode, parent->inode_num);
    rewrite_bgd(fs);
}

void EXT2_unlink(vfs_node *parent, char *name)
{
    ext2_fs *fs = (ext2_fs *)parent->device;
    EXT2_removeEntry(parent, name);

    inode_t *p_inode = (inode_t *)kmalloc(sizeof(inode_t));
    read_inode_metadata(fs, p_inode, parent->inode_num);
    p_inode->hard_links--;
    write_inode_metadata(fs, p_inode, parent->inode_num);
    rewrite_bgd(fs);
}

char **EXT2_listdir(vfs_node *parent)
{
    ext2_fs * ext2fs = parent->device;
    inode_t * p_inode = kmalloc(sizeof(inode_t));
    read_inode_metadata(ext2fs, p_inode, parent->inode_num);
    uint32_t curr_offset = 0;
    uint32_t block_offset = 0;
    uint32_t in_block_offset = 0;
    int size = 0, cap = 10;
    char ** ret = kmalloc(sizeof(char*) * cap);
    char * block_buf = read_inode_block(ext2fs, p_inode, block_offset);
    while(curr_offset < p_inode->size)
    {
        if(in_block_offset >= ext2fs->block_size)
        {
            block_offset++;
            in_block_offset = 0;
            block_buf = read_inode_block(ext2fs, p_inode, block_offset);
        }
        if(size + 1 == cap)
        {
            ret = krealloc(ret, sizeof(char*) * cap * 2);
            cap = cap * 2;
        }

        Dirent * curr_dir = (Dirent*)(block_buf + in_block_offset);
        if(curr_dir->inode != 0) {
            char * temp = kcalloc(curr_dir->name_len + 1, 1);
            memcpy(temp, curr_dir->name, curr_dir->name_len);
            ret[size++] = temp;
        }
        uint32_t expected_size = ((sizeof(Dirent) + curr_dir->name_len) & 0xfffffffc) + 0x4;
        uint32_t real_size = curr_dir->rec_len;
        if(real_size != expected_size)
        {
            break;
        }
        in_block_offset += curr_dir->rec_len;
        curr_offset += curr_dir->rec_len;
    }
    ret[size] = NULL;
    return ret;
}

void EXT2_chmod(vfs_node *node, uint32_t mode)
{
    ext2_fs *fs = (ext2_fs*)node->device;
    inode_t *inode = (inode_t*)kmalloc(sizeof(inode_t));
    read_inode_metadata(fs, inode, node->inode_num);
    inode->permission = (inode->permission & 0xFFFFF000) | mode;
    write_inode_metadata(fs, inode, node->inode_num);
}

void EXT2_createEntry(vfs_node *parent, char *name, uint32_t entry_inode)
{
    ext2_fs *fs = (ext2_fs*)parent->device;
    inode_t *p_inode = (inode_t*)kmalloc(sizeof(inode_t));
    read_inode_metadata(fs, p_inode, parent->inode_num);
    uint32_t curr_offset = 0;
    uint32_t block_offset = 0;
    uint32_t in_block_offset = 0;
    uint32_t found = 0;
    uint32_t entry_name_len = strlen(name);
    char *check = (char*)kcalloc(entry_name_len + 1, 1);
    char *block_buf = read_inode_block(fs, p_inode, block_offset);

    while (curr_offset < p_inode->size)
    {
        if (in_block_offset >= fs->block_size)
        {
            block_offset++;
            in_block_offset = 0;
            block_buf = read_inode_block(fs, p_inode, block_offset);
        }
        Dirent *c_dir = (Dirent *)(block_buf + in_block_offset);
        if (c_dir->name_len == entry_name_len)
        {
            memcpy(check, c_dir->name, entry_name_len);
            if (c_dir->inode != 0 && !strcmp(name, check))
            {
                printf("Entry by the same name %s already exist\n", check);
                return;
            }
        }

        if (found)
        {
            c_dir->inode = entry_inode;
            c_dir->rec_len = (uint32_t)block_buf + fs->block_size - (uint32_t)c_dir;
            c_dir->name_len = strlen(name);
            c_dir->file_type = 0;

            memcpy(c_dir->name, name, strlen(name));
            write_inode_block(fs, p_inode, block_offset, block_buf);

            in_block_offset += c_dir->rec_len;

            if (in_block_offset >= fs->block_size)
            {
                block_offset++;
                in_block_offset = 0;
                block_buf = read_inode_block(fs, p_inode, block_offset);
            }

            c_dir = (Dirent *)(block_buf + in_block_offset);

            memset(c_dir, 0, sizeof(Dirent));
            write_inode_block(fs, p_inode, block_offset, block_buf);

            return;
        }
        uint32_t expected_size = ((sizeof(Dirent) + c_dir->name_len) & 0xfffffffc) + 0x4;
        uint32_t real_size = c_dir->rec_len;
        if (real_size != expected_size)
        {
            found = 1;
            c_dir->rec_len = expected_size;
            in_block_offset += expected_size;
            curr_offset += expected_size;
            continue;
        }
        in_block_offset += c_dir->rec_len;
        curr_offset += c_dir->rec_len;
    }
}

void EXT2_removeEntry(vfs_node *parent, char *name)
{
    ext2_fs *fs = (ext2_fs*)parent->device;
    inode_t *p_inode = (inode_t*)kmalloc(sizeof(inode_t));
    read_inode_metadata(fs, p_inode, parent->inode_num);
    uint32_t curr_offset = 0;
    uint32_t block_offset = 0;
    uint32_t in_block_offset = 0;
    uint32_t entry_name_len = strlen(name);
    char *check = (char*)kcalloc(entry_name_len + 1, 1);
    char *block_buf = read_inode_block(fs, p_inode, block_offset);

    while (curr_offset < p_inode->size)
    {
        if (in_block_offset >= fs->block_size)
        {
            block_offset++;
            in_block_offset = 0;
            block_buf = read_inode_block(fs, p_inode, block_offset);
        }
        Dirent *c_dir = (Dirent *)(block_buf + in_block_offset);
        if (c_dir->name_len == entry_name_len)
        {
            memcpy(check, c_dir->name, entry_name_len);
            if (c_dir->inode != 0 && !strcmp(name, check))
            {
                c_dir->inode = 0;
                write_inode_block(fs, p_inode, block_offset, block_buf);
                return;
            }
        }
        uint32_t expected_size = ((sizeof(Dirent) + c_dir->name_len) & 0xfffffffc) + 0x4;
        uint32_t real_size = c_dir->rec_len;

        if (real_size != expected_size)
            return;
        in_block_offset += c_dir->rec_len;
        curr_offset += c_dir->rec_len;
    }
}

uint32_t EXT2_read(vfs_node *file, uint32_t offset, uint32_t size, char *buffer)
{
    ext2_fs *fs = (ext2_fs*)file->device;
    inode_t *inode = (inode_t*)kmalloc(sizeof(inode_t));
    read_inode_metadata(fs, inode, file->inode_num);
    read_inode_filedata(fs, inode, offset, size, buffer);
    return size;
}

uint32_t EXT2_write(vfs_node *file, uint32_t offset, uint32_t size, char *buffer)
{
    ext2_fs *fs = (ext2_fs *)file->device;
    inode_t *inode = (inode_t *)kmalloc(sizeof(inode_t));
    read_inode_metadata(fs, inode, file->inode_num);
    write_inode_filedata(fs, inode, file->inode_num, offset, size, buffer);
    return size;
}

void EXT2_open(vfs_node *file, uint32_t flags)
{
    ext2_fs *fs = (ext2_fs*)file->device;

    if (flags & OPEN_TRUNC)
    {
        inode_t *inode = (inode_t*)kmalloc(sizeof(inode_t));
        read_inode_metadata(fs, inode, file->inode_num);
        inode->size = 0;
        write_inode_metadata(fs, inode, file->inode_num);
    }
}

void EXT2_close(vfs_node *file)
{
    return;
}

void read_inode_metadata(ext2_fs *fs, inode_t *inode, uint32_t inode_num)
{
    uint32_t group = inode_num / fs->inodes_per_group;
    uint32_t inode_table_block = fs->bgd[group].inode_table;
    uint32_t idx_in_group = inode_num - group * fs->inodes_per_group;
    uint32_t block_offset = (idx_in_group - 1) * fs->sb->inode_size / fs->block_size;
    uint32_t offset_in_block = (idx_in_group - 1) - block_offset * (fs->block_size / fs->sb->inode_size);
    char *block_buf = (char *)kmalloc(fs->block_size);
    read_disk_block(fs, inode_table_block + block_offset, block_buf);
    memcpy(inode, block_buf + offset_in_block * fs->sb->inode_size, fs->sb->inode_size);
    kfree(block_buf);
}

void write_inode_metadata(ext2_fs *fs, inode_t *inode, uint32_t inode_num)
{
    uint32_t group = inode_num / fs->inodes_per_group;
    uint32_t inode_table_block = fs->bgd[group].inode_table;
    uint32_t block_offset = (inode_num - 1) * fs->sb->inode_size / fs->block_size;
    uint32_t offset_in_block = (inode_num - 1) - block_offset * (fs->block_size / fs->sb->inode_size);
    char *block_buf = (char *)kmalloc(fs->block_size);
    read_disk_block(fs, inode_table_block + block_offset, block_buf);
    memcpy(block_buf + offset_in_block * fs->sb->inode_size, inode, fs->sb->inode_size);
    write_disk_block(fs, inode_table_block + block_offset, block_buf);
    kfree(block_buf);
}

uint32_t read_inode_filedata(ext2_fs *fs, inode_t *inode, uint32_t offset, uint32_t size, char *buf)
{
    uint32_t end_offset = (inode->size >= offset + size) ? (offset + size) : (inode->size);
    uint32_t start_block = offset / fs->block_size;
    uint32_t end_block = end_offset / fs->block_size;
    uint32_t start_off = offset % fs->block_size;
    uint32_t end_size = end_offset - end_block * fs->block_size;

    uint32_t i = start_block;
    uint32_t curr_off = 0;
    while (i <= end_block)
    {
        uint32_t left = 0, right = fs->block_size - 1;
        char *block_buf = read_inode_block(fs, inode, i);
        if (i == start_block)
            left = start_off;
        if (i == end_block)
            right = end_size - 1;
        memcpy(buf + curr_off, block_buf + left, (right - left + 1));
        curr_off = curr_off + (right - left + 1);
        kfree(block_buf);
        i++;
    }
    return end_offset - offset;
}

void write_inode_filedata(ext2_fs *fs, inode_t *inode, uint32_t inode_num, uint32_t offset, uint32_t size, char *buf)
{
    if (offset + size > inode->size)
    {
        inode->size = offset + size;
        write_inode_metadata(fs, inode, inode_num);
    }
    uint32_t end_offset = (inode->size >= offset + size) ? (offset + size) : (inode->size);
    uint32_t start_block = offset / fs->block_size;
    uint32_t end_block = end_offset / fs->block_size;
    uint32_t start_off = offset % fs->block_size;
    uint32_t end_size = end_offset - end_block * fs->block_size;

    uint32_t i = start_block;
    uint32_t curr_off = 0;
    while (i <= end_block)
    {
        uint32_t left = 0, right = fs->block_size;
        char *block_buf = read_inode_block(fs, inode, i);

        if (i == start_block)
            left = start_off;
        if (i == end_block)
            right = end_size - 1;
        memcpy(block_buf + left, buf + curr_off, (right - left + 1));
        curr_off = curr_off + (right - left + 1);
        write_inode_block(fs, inode, i, block_buf);
        kfree(block_buf);
        i++;
    }
}

char *read_inode_block(ext2_fs *fs, inode_t *inode, uint32_t iblock)
{
    char *buf = (char *)kmalloc(fs->block_size);
    uint32_t DBlock_num = get_DBlock_num(fs, inode, iblock);
    read_disk_block(fs, DBlock_num, buf);
    return buf;
}

void write_inode_block(ext2_fs *fs, inode_t *inode, uint32_t iblock, char *buf)
{
    uint32_t DBlock_num = get_DBlock_num(fs, inode, iblock);
    write_disk_block(fs, DBlock_num, buf);
}

void read_disk_block(ext2_fs *fs, uint32_t block, char *buf)
{
    VFS_read(fs->device, fs->block_size * block, fs->block_size, buf);
}

void write_disk_block(ext2_fs *fs, uint32_t block, char *buf)
{
    VFS_write(fs->device, fs->block_size * block, fs->block_size, buf);
}

void rewrite_bgd(ext2_fs *fs)
{
    for (uint32_t i = 0; i < fs->bgd_block; i++)
        write_disk_block(fs, 2, (char *)fs->bgd + i * fs->block_size);
}

void rewrite_superblock(ext2_fs *fs)
{
    write_disk_block(fs, 1, (char *)fs->sb);
}

int alloc_inode_metadata_block(uint32_t *block_ptr, ext2_fs *fs, inode_t *inode, uint32_t inode_num, char *buffer, uint32_t block_overwrite)
{
    if (!(*block_ptr))
    {
        uint32_t block_num = EXT2_alloc_block(fs);
        if (!block_num)
            return 0;
        *block_ptr = block_num;
        if (buffer)
        {
            write_disk_block(fs, block_num, buffer);
        }
        else
        {
            write_inode_metadata(fs, inode, inode_num);
        }
        return 1;
    }
    return 0;
}

uint32_t get_DBlock_num(ext2_fs *fs, inode_t *inode, uint32_t IBlock_num)
{
    uint32_t p = fs->block_size / 4, res;
    int a, b, c, d, e, f, g, h;

    uint32_t *temp = (uint32_t *)kmalloc(fs->block_size);
    a = IBlock_num - EXT2_DIRECT_BLOCKS;

    if (a < 0)
    {
        res = inode->blocks[IBlock_num];
        goto exit;
    }

    b = a - p;
    if (b < 0)
    {
        read_disk_block(fs, inode->blocks[EXT2_DIRECT_BLOCKS], (char *)temp);
        res = temp[a];
        goto exit;
    }

    c = b - p * p;
    if (c < 0)
    {
        c = b / p;
        d = b - c * p;
        read_disk_block(fs, inode->blocks[EXT2_DIRECT_BLOCKS + 1], (char *)temp);
        read_disk_block(fs, temp[c], (char *)temp);
        res = temp[d];
        goto exit;
    }

    d = c - p * p * p;
    if (d < 0)
    {
        e = c / (p * p);
        f = (c - e * p * p) / p;
        g = (c - e * p * p - f * p);
        read_disk_block(fs, inode->blocks[EXT2_DIRECT_BLOCKS + 2], (char *)temp);
        read_disk_block(fs, temp[e], (char *)temp);
        read_disk_block(fs, temp[f], (char *)temp);
        res = temp[g];
        goto exit;
    }

exit:
    kfree(temp);
    return res;
}

void set_DBlock_num(ext2_fs *fs, inode_t *inode, uint32_t inode_num, uint32_t IBlock_num, uint32_t DBlock_num)
{
    uint32_t p = fs->block_size / 4;
    int a, b, c, d, e, f, g;
    int iblock = IBlock_num;
    uint32_t *temp = (uint32_t *)kmalloc(fs->block_size);

    a = iblock - EXT2_DIRECT_BLOCKS;
    if (a <= 0)
    {
        inode->blocks[IBlock_num] = DBlock_num;
        goto exit;
    }

    b = a - p;
    if (b <= 0)
    {
        if (!alloc_inode_metadata_block(&(inode->blocks[EXT2_DIRECT_BLOCKS]), fs, inode, inode_num, NULL, 0))
            ;
        read_disk_block(fs, inode->blocks[EXT2_DIRECT_BLOCKS], (char *)temp);
        ((uint32_t *)temp)[a] = DBlock_num;
        write_disk_block(fs, inode->blocks[EXT2_DIRECT_BLOCKS], (char *)temp);
        temp[a] = DBlock_num;
        goto exit;
    }

    c = b - p * p;
    if (c <= 0)
    {
        c = b / p;
        d = b - c * p;
        if (!alloc_inode_metadata_block(&(inode->blocks[EXT2_DIRECT_BLOCKS + 1]), fs, inode, inode_num, NULL, 0))
            ;
        read_disk_block(fs, inode->blocks[EXT2_DIRECT_BLOCKS + 1], (char *)temp);
        if (!alloc_inode_metadata_block(&(temp[c]), fs, inode, inode_num, (char *)temp, inode->blocks[EXT2_DIRECT_BLOCKS + 1]))
            ;
        uint32_t tmp = temp[c];
        read_disk_block(fs, tmp, (char *)temp);
        temp[d] = DBlock_num;
        write_disk_block(fs, tmp, (char *)temp);
        goto exit;
    }

    d = c - p * p * p;
    if (d <= 0)
    {
        e = c / (p * p);
        f = (c - e * p * p) / p;
        g = (c - e * p * p - f * p);
        if (!alloc_inode_metadata_block(&(inode->blocks[EXT2_DIRECT_BLOCKS + 2]), fs, inode, inode_num, NULL, 0))
            ;
        read_disk_block(fs, inode->blocks[EXT2_DIRECT_BLOCKS + 2], (char *)temp);
        if (!alloc_inode_metadata_block(&(temp[e]), fs, inode, inode_num, (char *)temp, inode->blocks[EXT2_DIRECT_BLOCKS + 2]))
            ;
        uint32_t tmp = temp[e];
        read_disk_block(fs, temp[e], (char *)temp);
        if (!alloc_inode_metadata_block(&(temp[f]), fs, inode, inode_num, (char *)temp, tmp))
            ;
        tmp = temp[f];
        read_disk_block(fs, temp[f], (char *)temp);
        temp[g] = DBlock_num;
        write_disk_block(fs, tmp, (char *)temp);
        goto exit;
    }

exit:
    kfree(temp);
}

uint32_t EXT2_alloc_block(ext2_fs *fs)
{
    uint32_t *buf = (uint32_t *)kcalloc(fs->block_size, 1);
    for (uint32_t i = 0; i < fs->total_groups; i++)
    {
        if (!fs->bgd[i].free_blocks_count)
            continue;

        uint32_t bitmap_block = fs->bgd[i].block_bitmap;
        read_disk_block(fs, bitmap_block, (char *)buf);
        for (uint32_t j = 0; j < fs->block_size / 4; j++)
        {
            uint32_t sub_bitmap = buf[j];
            if (sub_bitmap == 0xFFFFFFFF)
                continue;
            for (uint32_t k = 0; k < 32; k++)
            {
                uint32_t free = !((sub_bitmap >> k) & 0x1);
                if (free)
                {
                    uint32_t mask = (0x1 << k);
                    buf[j] = buf[j] | mask;
                    write_disk_block(fs, bitmap_block, (char *)buf);
                    fs->bgd[i].free_blocks_count--;
                    rewrite_bgd(fs);
                    return i * fs->block_per_group + j * 32 + k;
                }
            }
        }
    }
    printf("Out of blocks for allocation!\n");
    return (uint32_t)-1;
}

void EXT2_free_block(ext2_fs *fs, uint32_t block_num)
{
    uint32_t *buf = (uint32_t *)kcalloc(fs->block_size, 1);
    uint32_t group_num = block_num / fs->block_per_group;
    uint32_t sub_Bitmap_num = (block_num - (fs->block_per_group * group_num)) / 4;
    uint32_t index = (block_num - (fs->block_per_group * group_num)) % 4;

    uint32_t bitmap_block = fs->bgd[group_num].block_bitmap;
    read_disk_block(fs, bitmap_block, (char *)buf);

    uint32_t mask = ~(0x1 << index);
    buf[sub_Bitmap_num] = buf[sub_Bitmap_num] & mask;

    write_disk_block(fs, bitmap_block, (char *)buf);

    fs->bgd[group_num].free_blocks_count++;
    rewrite_bgd(fs);
}

void alloc_IBlock(ext2_fs *fs, inode_t *inode, uint32_t inode_num, uint32_t block)
{
    uint32_t res = EXT2_alloc_block(fs);
    set_DBlock_num(fs, inode, inode_num, block, res);
    inode->num_sectors = (block + 1) * (fs->block_size / 512);

    write_inode_metadata(fs, inode, inode_num);
}

void free_IBlock(ext2_fs *fs, inode_t *inode, uint32_t inode_num, uint32_t block)
{
    uint32_t res = get_DBlock_num(fs, inode, block);
    EXT2_free_block(fs, res);
    set_DBlock_num(fs, inode, inode_num, res, 0);
    write_inode_metadata(fs, inode, inode_num);
}

uint32_t alloc_inode(ext2_fs *fs)
{
    uint32_t *buf = (uint32_t *)kcalloc(fs->block_size, 1);

    for (uint32_t i = 0; i < fs->total_groups; i++)
    {
        if (!fs->bgd[i].free_inodes_count)
            continue;

        uint32_t bitmap_block = fs->bgd[i].inode_bitmap;
        read_disk_block(fs, bitmap_block, (char *)buf);
        for (uint32_t j = 0; j < fs->block_size / 4; j++)
        {
            uint32_t sub_bitmap = buf[j];
            if (sub_bitmap == 0xFFFFFFFF)
                continue;
            for (uint32_t k = 0; k < 32; k++)
            {
                uint32_t free = !((sub_bitmap >> k) & 0x1);
                if (free)
                {
                    uint32_t mask = (0x1 << k);
                    buf[j] = buf[j] | mask;
                    write_disk_block(fs, bitmap_block, (char *)buf);
                    fs->bgd[i].free_inodes_count--;
                    rewrite_bgd(fs);
                    return i * fs->inodes_per_group + j * 32 + k;
                }
            }
        }
    }
    printf("out of inodes!\n");
    return (uint32_t)-1;
}

void free_inode(ext2_fs *fs, uint32_t inode_num)
{
    uint32_t *buf = (uint32_t *)kcalloc(fs->block_size, 1);
    uint32_t group_num = inode_num / fs->inodes_per_group;
    uint32_t sub_Bitmap_num = (inode_num - (fs->inodes_per_group * group_num)) / 4;
    uint32_t index = (inode_num - (fs->inodes_per_group * group_num)) % 4;

    uint32_t bitmap_block = fs->bgd[group_num].inode_bitmap;
    read_disk_block(fs, bitmap_block, (char *)buf);

    uint32_t mask = ~(0x1 << index);
    buf[sub_Bitmap_num] = buf[sub_Bitmap_num] & mask;

    write_disk_block(fs, bitmap_block, (char *)buf);

    fs->bgd[group_num].free_inodes_count++;
    rewrite_bgd(fs);
}

vfs_node *EXT2_get_root(ext2_fs *fs, inode_t *inode)
{
    vfs_node *root = (vfs_node *)kcalloc(sizeof(vfs_node), 1);
    strcpy(root->name, "/");
    root->device = fs;
    root->mask = inode->permission;
    root->inode_num = ROOT_INODE_NUMBER;
    root->size = inode->size;

    root->last_accessed = inode->atime;
    root->last_modified = inode->mtime;
    root->creation_time = inode->ctime;

    root->flags |= FS_DIRECTORY;

    root->read    = NULL;
    root->write   = NULL;
    root->chmod   = EXT2_chmod;
    root->open    = EXT2_open;
    root->close   = EXT2_close;
    root->read    = EXT2_read;
    root->write   = EXT2_write;
    root->mkdir   = EXT2_mkdir;
    root->create  = EXT2_mkfile;
    root->listdir = EXT2_listdir;
    root->finddir = EXT2_finddir;
    root->unlink  = EXT2_unlink;
}

vfs_node *vfsnode_from_direntry(ext2_fs *fs, Dirent *dir, inode_t *inode)
{
    vfs_node *res = (vfs_node *)kcalloc(sizeof(vfs_node), 1);

    res->device = (void *)fs;
    res->inode_num = dir->inode;
    memcpy(res->name, dir->name, dir->name_len);

    res->uid = inode->uid;
    res->uid = inode->gid;
    res->size = inode->size;
    res->mask = inode->permission & 0xFFF;
    res->nlink = inode->hard_links;

    res->flags = 0;
    if ((inode->permission & EXT2_IFREG) == EXT2_IFREG)
    {
        res->flags |= FS_FILE;
        res->read = EXT2_read;
        res->write = EXT2_write;
        res->unlink = EXT2_unlink;
        res->get_filesize = EXT2_getFileSize;
    }
    if ((inode->permission & EXT2_IFDIR) == EXT2_IFDIR)
    {
        res->flags |= FS_DIRECTORY;
        res->mkdir = EXT2_mkdir;
        res->finddir = EXT2_finddir;
        res->unlink = EXT2_unlink;
        res->create = EXT2_mkfile;
        res->listdir = EXT2_listdir;
        res->read = EXT2_read;
        res->write = EXT2_write;
    }
    if ((inode->permission & EXT2_IFBLK) == EXT2_IFBLK)
    {
        res->flags |= FS_BLOCKDEVICE;
    }
    if ((inode->permission & EXT2_IFCHR) == EXT2_IFCHR)
    {
        res->flags |= FS_CHARDEVICE;
    }
    if ((inode->permission & EXT2_IFIFO) == EXT2_IFIFO)
    {
        res->flags |= FS_PIPE;
    }
    if ((inode->permission & EXT2_IFLNK) == EXT2_IFLNK)
    {
        res->flags |= FS_SYMLINK;
    }

    res->last_accessed = inode->atime;
    res->last_modified = inode->mtime;
    res->creation_time = inode->ctime;

    res->chmod = EXT2_chmod;
    res->open = EXT2_open;
    res->close = EXT2_close;
    return res;
}

vfs_node *EXT2_finddir(vfs_node *parent, char *name)
{
    ext2_fs *fs = (ext2_fs *)parent->device;

    inode_t *p_inode = (inode_t *)kmalloc(sizeof(inode_t));
    read_inode_metadata(fs, p_inode, parent->inode_num);

    uint32_t expected_size;
    uint32_t real_size;
    uint32_t curr_offset = 0;
    uint32_t block_offset = 0;
    uint32_t in_block_offset = 0;

    char *block_buf = read_inode_block(fs, p_inode, block_offset);

    while (curr_offset < p_inode->size)
    {
        if (in_block_offset >= fs->block_size)
        {
            block_offset++;
            in_block_offset = 0;
            block_buf = read_inode_block(fs, p_inode, block_offset);
        }

        Dirent *c_dir = (Dirent *)(block_buf + in_block_offset);

        char *temp = (char *)kcalloc(c_dir->name_len + 1, 1);

        memcpy(temp, c_dir->name, c_dir->name_len);

        if (c_dir->inode != 0 && !strcmp(temp, name))
        {
            inode_t *inode = (inode_t *)kmalloc(sizeof(inode_t));
            read_inode_metadata(fs, inode, c_dir->inode);
            return vfsnode_from_direntry(fs, c_dir, inode);
        }

        if (((sizeof(Dirent) + c_dir->name_len) & 0x00000003) != 0)
            expected_size = ((sizeof(Dirent) + c_dir->name_len) & 0xfffffffc) + 0x4;
        else
            expected_size = ((sizeof(Dirent) + c_dir->name_len) & 0xfffffffc);
        real_size = c_dir->rec_len;

        if (real_size != expected_size)
        {
            break;
        }

        in_block_offset += c_dir->rec_len;
        curr_offset += c_dir->rec_len;
    }
    return NULL;
}


void ext2_DebugPrintInode(inode_t* inode, ext2_fs* fs)
{
    serial_printf("Inode is %s:\n", isAllZero(inode, fs->sb->inode_size) ? "Empty" : "Not Empty");
    serial_printf("Mode: %d\n", inode->permission);
    serial_printf("UID: %d\n", inode->uid);
    serial_printf("GID: %d\n", inode->gid);
    serial_printf("Size: %d\n", inode->size);
    serial_printf("num_sectors: %d\n", inode->num_sectors);
    serial_printf("Links: %d\n", inode->hard_links);
    serial_printf("Blocks: %d\n", inode->blocks);
    serial_printf("Flags: %d\n\n", inode->flags);
}

void ext2_DebugPrintBGD(ext2_fs* bgd)
{
    serial_printf("BGD:\n");
    serial_printf("Block bitmap: %d\n", bgd->bgd->block_bitmap);
    serial_printf("Inode bitmap: %d\n", bgd->bgd->inode_bitmap);
    serial_printf("Inode table: %d\n", bgd->bgd->inode_table);
    serial_printf("Free blocks: %d\n", bgd->bgd->free_blocks_count);
    serial_printf("Free inodes: %d\n", bgd->bgd->free_inodes_count);
    serial_printf("Used dir count: %d\n", bgd->bgd->used_dirs_count);
}

void ext2_DebugPrintSB(ext2_fs* ext2fs)
{
    serial_printf("Superblock:\n");
    serial_printf("Inodes: %d\n", ext2fs->sb->total_inodes);
    serial_printf("Blocks: %d\n", ext2fs->sb->total_blocks);
    serial_printf("Reserved blocks: %d\n", ext2fs->sb->reserved_blocks);
    serial_printf("Free blocks: %d\n", ext2fs->sb->free_blocks);
    serial_printf("Free inodes: %d\n", ext2fs->sb->free_inodes);
    serial_printf("First data block: %d\n", ext2fs->sb->first_data_block);
    serial_printf("Log2 of block size: %d\n", ext2fs->sb->log_block_size);
    serial_printf("Log2 of fragment size: %d\n", ext2fs->sb->log_frag_size);
    serial_printf("Blocks per group: %d\n", ext2fs->sb->blocks_per_group);
    serial_printf("Fragments per group: %d\n", ext2fs->sb->frags_per_group);
    serial_printf("Inodes per group: %d\n", ext2fs->sb->inodes_per_group);
    serial_printf("Last mount time: %d\n", ext2fs->sb->mtime);
    serial_printf("Last write time: %d\n", ext2fs->sb->wtime);
    serial_printf("Mount count: %d\n", ext2fs->sb->mnt_count);
    serial_printf("Max mount count: %d\n", ext2fs->sb->max_mnt_count);
    serial_printf("Magic: %d [%s]\n", ext2fs->sb->magic, (ext2fs->sb->magic == VFS_EXT2_MAGIC) ? "OK" : "FAIL");
    serial_printf("State: %d [%s]\n", ext2fs->sb->state);
    serial_printf("Errors: %d [%s]\n", ext2fs->sb->errors, (ext2fs->sb->errors == 0) ? "OK" : (ext2fs->sb->errors == 1) ? "IGNORED" : (ext2fs->sb->errors == 2) ? "REMOUNT RO" : "KERNEL PANIC");
    serial_printf("Last mount dir: %s\n", ext2fs->sb->last_mounted);
}

int isExt2(char* devpath)
{
    ext2_fs *fs = (ext2_fs *)kcalloc(sizeof(ext2_fs), 1);
    fs->device = file_open(devpath, 0);

    fs->sb = (superblock*)kmalloc(SUPERBLOCK_SIZE);

    fs->block_size = 1024;

    read_disk_block(fs, 1, (char *)fs->sb);
    if(fs->sb->magic != VFS_EXT2_MAGIC)
    {
        kfree(fs);
        return 0;
    }

    VFS_close(fs->device);

    kfree(fs);
    return 1;
}

void EXT2_init(char *dev_path, char *mountpoint)
{
    ext2_fs *fs = (ext2_fs *)kcalloc(sizeof(ext2_fs), 1);
    fs->device = file_open(dev_path, 0);

    fs->sb = (superblock*)kmalloc(SUPERBLOCK_SIZE);

    fs->block_size = 1024;

    read_disk_block(fs, 1, (char *)fs->sb);
    ext2_DebugPrintSB(fs);
    xxd(fs->sb, SUPERBLOCK_SIZE);
    if(fs->sb->magic != VFS_EXT2_MAGIC)
    {
        printf("EXT2: Invalid magic number\n");
        return;
    }
    fs->block_size = (1024 << fs->sb->log_block_size);
    fs->block_per_group = fs->sb->blocks_per_group;
    fs->inodes_per_group = fs->sb->inodes_per_group;
    fs->total_groups = fs->sb->total_blocks / fs->block_per_group;

    if(fs->block_per_group * fs->total_groups < fs->total_groups)
        fs->total_groups++;

    fs->bgd_block = (fs->total_groups * sizeof(block_group_descriptor)) / fs->block_size;
    if (fs->bgd_block * fs->block_size < fs->total_groups * sizeof(block_group_descriptor))
        fs->bgd_block++;

    fs->bgd = (block_group_descriptor*)kcalloc(sizeof(block_group_descriptor), fs->bgd_block * fs->block_size);
    for (uint32_t i = 0; i < fs->bgd_block; i++)
    {
        read_disk_block(fs, 2, (char *)fs->bgd + i * fs->block_size);
    }

    inode_t *root_inode = (inode_t*)kcalloc(sizeof(inode_t), 1);
    read_inode_metadata(fs, root_inode, ROOT_INODE_NUMBER);
    ext2_DebugPrintInode(root_inode, fs);
    xxd(root_inode, fs->sb->inode_size);

    VFS_mount(mountpoint, EXT2_get_root(fs, root_inode));
    printf("EXT2: Mounted %s on %s\n", dev_path, mountpoint);
    return;
}