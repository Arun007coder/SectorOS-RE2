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

#ifndef __BITMAP_H
#define __BITMAP_H

#include <system.h>
#include <vfs.h>
#include <string.h>
#include <vesa.h>
#include <kheap.h>

typedef struct BITMAP_FILE_HEADER
{
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
} __attribute__((packed)) BITMAP_FILE_HEADER_t;

typedef struct BITMAP_INFO_HEADER
{
    uint32_t biSize;
    long biWidth;
    long biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    long biXPelsPerMeter;
    long biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} __attribute__((packed)) BITMAP_INFO_HEADER_t;

typedef struct BITMAP
{
    uint32_t width;
    uint32_t height;
    uint32_t bpp;
    char* image_bytes;
    char* buffer;
    uint32_t total_size;
}BITMAP_t;

typedef struct palette
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
}palette_t;

BITMAP_t * bitmap_create(char * filename);

void bitmap_display(char* file);

void bitmap_to_framebuffer(BITMAP_t * bmp, uint32_t * frame_buffer);
void bitmap_to_framebuffer2(BITMAP_t * bmp, uint32_t * frame_buffer);

#endif