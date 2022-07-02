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

#include "mbr.h"

void probe_partitions(char* device)
{
    printf("[MBR] Probing partitions on %s\n", device);
    FILE* f = file_open(device, 0);
    if(f == NULL)
    {
        printf("[MBR] Could not open %s\n", device);
        return;
    }
    
    MBR_t* mbr = kmalloc(512);
    VFS_read(f, 0, 512, (char*)mbr);
    VFS_close(f);

    if(mbr->magicnumber != 0xAA55)
    {
        printf("[MBR] Invalid MBR\n");
        return;
    }

    FILE* part1Node = kmalloc(sizeof(FILE));
    char* part1Path = kmalloc(sizeof(char) * (strlen("/dev/") + 20));

    FILE* part2Node = kmalloc(sizeof(FILE));
    char* part2Path = kmalloc(sizeof(char) * (strlen("/dev/") + 20));

    FILE* part3Node = kmalloc(sizeof(FILE));
    char* part3Path = kmalloc(sizeof(char) * (strlen("/dev/") + 20));

    FILE* part4Node = kmalloc(sizeof(FILE));
    char* part4Path = kmalloc(sizeof(char) * (strlen("/dev/") + 20));

    FILE* part5Node = kmalloc(sizeof(FILE));
    char* part5Path = kmalloc(sizeof(char) * (strlen("/dev/") + 20));

    if(mbr->primaryPartition[0].partition_id != 0x00)
    {
        partition_t* partition0 = kmalloc(sizeof(partition_t));
        partition0->num = 0;
        partition0->bootable = mbr->primaryPartition[0].bootable;
        partition0->partition_id = mbr->primaryPartition[0].partition_id;
        partition0->start_lba = mbr->primaryPartition[0].start_lba;
        partition0->length = mbr->primaryPartition[0].length;
        partition0->device = file_open(device, 0);
        if(partition0->device == NULL)
        {
            printf("[MBR] Could not open %s\n", device);
            kfree(partition0);
            return;
        }
        printf("[MBR] Found primary partition 1 on %s\n", device);
        part1Node = mbr_getVFSNode(partition0);
        devfs_add(part1Node);
        strcpy(part1Path, "/dev/");
        strcat(part1Path, part1Node->name);
        partition0->fs_type = find_fs(part1Path);
    }

    if(mbr->primaryPartition[1].partition_id != 0x00)
    {
        partition_t* partition1 = kmalloc(sizeof(partition_t));
        partition1->num = 1;
        partition1->bootable = mbr->primaryPartition[1].bootable;
        partition1->partition_id = mbr->primaryPartition[1].partition_id;
        partition1->start_lba = mbr->primaryPartition[1].start_lba;
        partition1->length = mbr->primaryPartition[1].length;
        partition1->device = file_open(device, 0);
        if(partition1->device == NULL)
        {
            printf("[MBR] Could not open %s\n", device);
            kfree(partition1);
            return;
        }
        printf("[MBR] Found primary partition 2 on %s\n", device);
        part2Node = mbr_getVFSNode(partition1);
        devfs_add(part2Node);
        strcpy(part2Path, "/dev/");
        strcat(part2Path, part2Node->name);
        partition1->fs_type = find_fs(part2Path);
    }

    if(mbr->primaryPartition[2].partition_id != 0x00)
    {
        partition_t* partition2 = kmalloc(sizeof(partition_t));
        partition2->num = 2;
        partition2->bootable = mbr->primaryPartition[2].bootable;
        partition2->partition_id = mbr->primaryPartition[2].partition_id;
        partition2->start_lba = mbr->primaryPartition[2].start_lba;
        partition2->length = mbr->primaryPartition[2].length;
        partition2->device = file_open(device, 0);
        if(partition2->device == NULL)
        {
            printf("[MBR] Could not open %s\n", device);
            kfree(partition2);
            return;
        }
        printf("[MBR] Found primary partition 3 on %s\n", device);
        part3Node = mbr_getVFSNode(partition2);
        devfs_add(part3Node);
        strcpy(part3Path, "/dev/");
        strcat(part3Path, part3Node->name);
        partition2->fs_type = find_fs(part3Path);
    }

    if(mbr->primaryPartition[3].partition_id != 0x00)
    {
        partition_t* partition3 = kmalloc(sizeof(partition_t));
        partition3->num = 3;
        partition3->bootable = mbr->primaryPartition[3].bootable;
        partition3->partition_id = mbr->primaryPartition[3].partition_id;
        partition3->start_lba = mbr->primaryPartition[3].start_lba;
        partition3->length = mbr->primaryPartition[3].length;
        partition3->device = file_open(device, 0);
        if(partition3->device == NULL)
        {
            printf("[MBR] Could not open %s\n", device);
            kfree(partition3);
            return;
        }
        printf("[MBR] Found primary partition 4 on %s\n", device);
        part4Node = mbr_getVFSNode(partition3);
        devfs_add(part4Node);
        strcpy(part4Path, "/dev/");
        strcat(part4Path, part4Node->name);
        partition3->fs_type = find_fs(part4Path);
    }

    if(mbr->primaryPartition[4].partition_id != 0x00)
    {
        partition_t* partition4 = kmalloc(sizeof(partition_t));
        partition4->num = 4;
        partition4->bootable = mbr->primaryPartition[4].bootable;
        partition4->partition_id = mbr->primaryPartition[4].partition_id;
        partition4->start_lba = mbr->primaryPartition[4].start_lba;
        partition4->length = mbr->primaryPartition[4].length;
        partition4->device = file_open(device, 0);
        if(partition4->device == NULL)
        {
            printf("[MBR] Could not open %s\n", device);
            kfree(partition4);
            return;
        }
        printf("[MBR] Found primary partition on %s\n", device);
        part5Node = mbr_getVFSNode(partition4);
        devfs_add(part5Node);
        strcpy(part5Path, "/dev/");
        strcat(part5Path, part5Node->name);
        partition4->fs_type = find_fs(part5Path);
    }

    kfree(part1Path);
    kfree(part2Path);
    kfree(part3Path);
    kfree(part4Path);
    kfree(part5Path);

    kfree(mbr);
}

FILE* mbr_getVFSNode(partition_t* partition)
{
    FILE* node = kmalloc(sizeof(FILE));
    strcpy(node->name, partition->device->name);
    strcat(node->name, "p");
    strcat(node->name, itoa_r(partition->num, 10));

    node->size = partition->length * 512;
    node->flags = FS_BLOCKDEVICE;
    node->device = partition;
    node->read = mbr_read;
    node->write = mbr_write;
    node->open = mbr_open;
    node->close = mbr_close;

    return node;
}

uint32_t mbr_read(FILE* node, uint32_t offset, uint32_t size, char* buffer)
{
    partition_t* partition = node->device;
    
    uint32_t toff = (partition->start_lba * 512) + offset;

    if(offset + size > (partition->length * 512))
    {
        printf("[MBR] Attempted to read past end of partition\n");
        return -1;
    }

    return VFS_read(partition->device, toff, size, buffer);
}

uint32_t mbr_write(FILE* node, uint32_t offset, uint32_t size, char* buffer)
{
    partition_t* partition = node->device;
    
    uint32_t toff = partition->start_lba + offset;
    if(offset + size > partition->length*512)
    {
        printf("[MBR] Attempted to write past end of partition\n");
        return -1;
    }

    return VFS_write(partition->device, toff, size, buffer);
}

void mbr_open(FILE* node, uint32_t flags)
{
    // Nothing to do
}

void mbr_close(FILE* node)
{
    // Nothing to do
}