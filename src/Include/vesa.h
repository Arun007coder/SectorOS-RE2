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

#ifndef __VESA_H__
#define __VESA_H__

#include <system.h>
#include <bios32.h>
#include <string.h>
#include <serial.h>

typedef struct VBE_INFO
{
    char signature[4];
    uint16_t version;
    uint32_t oem_string;
    uint32_t capabilities;
    uint32_t video_mode_list;
    uint16_t total_memory;
    uint16_t oem_software_rev;
    uint32_t oem_vendor_name;
    uint32_t oem_product_name;
    uint32_t oem_product_rev;
    uint32_t reserved[222];
    uint8_t oem_data[256];
}__attribute__((packed)) VBE_INFO_t;

typedef struct VBE_MODE_INFO
{
    uint16_t mode_attributes;
    uint8_t WindowA;
    uint8_t WindowB;
    uint16_t granularity;
    uint16_t window_size;
    uint16_t segment_a;
    uint16_t segment_b;
    uint32_t WindowFuncPtr;
    uint16_t pitch;

    uint16_t XResolution;
    uint16_t YResolution;
    uint8_t wChar;
    uint8_t yChar;
    uint8_t planes;
    uint8_t bpp;
    uint8_t banks;
    uint8_t memory_model;
    uint8_t bank_size;
    uint8_t image_pages;
    uint8_t reserved0;

    uint8_t redMask;
    uint8_t redPosition;
    uint8_t greenMask;
    uint8_t greenPosition;
    uint8_t blueMask;
    uint8_t bluePosition;
    uint8_t reservedMask;
    uint8_t reservedPosition;
    uint8_t directColor_attributes;

    uint32_t phys_base;
    uint32_t offscreen_mem_ptr;
    uint16_t offscreen_mem_size;
    uint8_t reserved[206];
}__attribute__((packed)) VBE_MODE_INFO_t;

typedef union uint24
{
        struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    }rgb;
    uint32_t integer : 24 __attribute__((packed));;
}uint24_t;

void init_vesa();

void vesa_memcpy24_to_32(uint24_t* dest, uint32_t* src, size_t size);
void vesa_memsetRGB(uint8_t * dest, uint32_t rgb, uint32_t count);

void* vesa_getFramebuffer();

int vesa_getXResolution();
int vesa_getYResolution();

bool isVesaInit();

void vesa_getModeInfo(uint32_t mode, VBE_MODE_INFO_t* modeInfo);
void vesa_setMode(uint32_t mode);

void vesa_putPixel(uint32_t x, uint32_t y, uint32_t color);

void vesa_change_mode(uint32_t width, uint32_t height, uint32_t bpp);
#endif