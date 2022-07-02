// Copyright (C) 2022 Arun007coder
// 
// This file is part of SectorOS-RE2.
// 
// SectorOS-RE2 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// SectorOS-RE2 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with SectorOS-RE2.  If not, see <http://www.gnu.org/licenses/>.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include "sorfs.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <input file name> <folder to extract file to>\n", argv[0]);
        return 1;
    }

    std::ifstream in(argv[1], std::ios::binary);
    if (!in.is_open())
    {
        std::cout << "Could not open file " << argv[1] << std::endl;
        return 1;
    }
    sorfs_superblock_t superblock;
    in.read((char*)&superblock, sizeof(sorfs_superblock_t));
    if (superblock.magic != SORFS_SB_MAGIC)
    {
        std::cout << "Invalid superblock magic" << std::endl;
        return 1;
    }

    printf("superblock %s info:\n", superblock.name);
    printf("size: %d\n", superblock.size);
    printf("superblock size: %d\n", sizeof(sorfs_superblock_t));
    printf("total blocks: %d\n", superblock.total_blocks);
    printf("free blocks: %d\n\n", superblock.free_blocks);

    char *path = new char[strlen(argv[2]) + 2];
    strcpy(path, argv[2]);
    strcat(path, "/");

    printf("saving to %s\n", path);

    if (chdir(path) != 0)
    {
        mkdir(path, 0777);
        chdir(path);
    }

    sorfs_block_t c_blocks[superblock.total_blocks];
    in.read((char*)c_blocks, (sizeof(c_blocks)));

    for(int i = 0; i < superblock.total_blocks; i++)
    {
        if(c_blocks[i].magic != SORFS_BLOCK_MAGIC)
        {
            std::cout << "Invalid block magic" << std::endl;
            return 1;
        }

        if(c_blocks[i].type != SORFS_TYPE_FRE)
        {
            printf("block %s info:\n", c_blocks[i].name);
            printf("type: %d\n", c_blocks[i].type);
            printf("size: %d\n", c_blocks[i].size);
            printf("offset: %d\n\n", c_blocks[i].offset);
            in.seekg(c_blocks[i].offset, std::ios::beg);
            char *data = new char[c_blocks[i].size];
            in.read((char*)data, c_blocks[i].size);
            printf("Contents:\n%s\n\n", data);
            std::ofstream out(c_blocks[i].name, std::ios::binary);
            out.write(data, c_blocks[i].size);
            out.close();
            delete[] data;
        }
    }


/*
    for (int i = 0; i < MAX_BLOCKS; i++)
    {
        if (blocks[i].magic == SORFS_BLOCK_MAGIC && blocks[i].type != SORFS_TYPE_FRE)
        {
            printf("%s info:\n", blocks[i].name);
            printf("size: %d\n", blocks[i].size);
            printf("type: %s\n", (blocks[i].type ? "Directory" : "File"));
            printf("disk offset: %d\n", blocks[i].offset);
            char buffer[blocks[i].size];
            in.seekg(blocks[i].offset, std::ios::beg);
            in.read(buffer, blocks[i].size);
            buffer[blocks[i].size] = '\0';
            printf("contents:\n\n%s\n", buffer);

            
        }
    }
    */

}