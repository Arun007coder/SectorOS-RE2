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
#include <strstream>
#include "sorfs.h"
#include <filesystem>

#define MAXLIST 100

uint32_t seek = 0;

void buffer_write(char *src, char* dest, uint32_t size)
{
    int j = 0;
    for (uint32_t i = seek; i < (size + seek) || j != size; i++)
    {
        dest[i] = src[j];
        j++;
    }
    seek += size;
}

int parseSpace(char *str, char **parsed)
{
    int i;

    for (i = 0; i < MAXLIST; i++)
    {
        parsed[i] = strsep(&str, " ");

        if (parsed[i] == NULL)
            break;
        if (strlen(parsed[i]) == 0)
            i--;
    }
    return i;
}

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        std::cout << "Usage: mksorfs <file> [<file> ...]" << std::endl;
        return 1;
    }
    char **file;
    file = (char**)malloc(sizeof(char*) * MAXLIST);
    for (int i = 2; i < argc; i++)
    {
        file[i - 2] = argv[i];
    }
    int argCount = argc - 2;

    char* output = argv[1];

    std::ofstream out(output, std::ios::binary);
    if (!out.is_open())
    {
        std::cout << "Could not open file " << argv[1] << std::endl;
        return 1;
    }
    uint32_t blocks_used = (argCount);
    sorfs_superblock_t superblock;
    superblock.magic = SORFS_SB_MAGIC;
    strcpy(superblock.name, output);
    superblock.size = sizeof(sorfs_superblock_t);
    superblock.total_blocks = MAX_BLOCKS;
    superblock.free_blocks = MAX_BLOCKS - blocks_used;
    out.write((char *)&superblock, sizeof(sorfs_superblock_t));
    sorfs_block_t c_blocks[MAX_BLOCKS];
    memset(c_blocks, 0, sizeof(c_blocks));
    uint32_t offset = sizeof(sorfs_superblock_t) + ((MAX_BLOCKS) * sizeof(sorfs_block_t));
    for (int i = 0; i < (argCount); i++)
    {
        std::ifstream in(file[i], std::ios::binary);
        if (!in.is_open())
        {
            std::cout << "Could not open file " << file[i] << std::endl;
            return 1;
        }
        in.seekg(0, std::ios::end);
        uint32_t size = in.tellg();
        in.seekg(0, std::ios::beg);
        
        c_blocks[i].magic = SORFS_BLOCK_MAGIC;
        strcpy(c_blocks[i].name, std::filesystem::path(file[i]).filename().string().c_str());
        c_blocks[i].type = SORFS_TYPE_REG;
        c_blocks[i].size = size;
        c_blocks[i].offset = offset;
        printf("%s %d %d %d\n", c_blocks[i].name, c_blocks[i].type, c_blocks[i].size, c_blocks[i].offset);
        offset += size;
    }
    for (int i = (argCount); i < MAX_BLOCKS; i++)
    {
        c_blocks[i].magic = SORFS_BLOCK_MAGIC;
        c_blocks[i].type = SORFS_TYPE_FRE;
        c_blocks[i].size = 0;
        c_blocks[i].offset = 0;
    }
    out.write((char *)c_blocks, sizeof(c_blocks));
    for (int i = 0; i < (argCount); i++)
    {
        std::ifstream in(file[i], std::ios::binary);
        if (!in.is_open())
        {
            std::cout << "Could not open file " << file[i] << std::endl;
            return 1;
        }
        in.seekg(0, std::ios::end);
        uint32_t size = in.tellg();
        in.seekg(0, std::ios::beg);
        char *buffer = new char[size];
        in.read(buffer, size);
        out.write(buffer, size);
        delete[] buffer;
    }
    out.close();
    free(file);
}