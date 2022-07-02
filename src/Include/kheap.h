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

// Based on code from http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20Heap.html
#ifndef _KHEAP_H_
#define _KHEAP_H_

#include "system.h"
#include "printf.h"
#include "string.h"
#include "vga.h"
#include "paging.h"

#define KHEAP_START (void*)0xC0400000
#define KHEAP_INITIAL_SIZE 48 * MB
#define KHEAP_MAX_ADDRESS (void*)0xCFFFFFFF
#define HEAP_MIN_SIZE 4 * MB

#define PAGE_SIZE 4096
#define OVERHEAD (sizeof(struct memory_block) + sizeof(uint32_t))

typedef struct memory_block
{
    struct memory_block* next;
    struct memory_block* prev;
    uint32_t size;
} memory_block_t;

extern bool Kheap_enabled;

/*
* if heap is initialized then uss the dynamic memory allocator else use the placement memory allocator
* @param size The size of the memory to be allocated
* @param align Should the memory be aligned?
* @param paddr Should the allocated memory be given a physical address?
*/
uint32_t kmalloc_int(uint32_t size, int align, uint32_t* paddr);

/*
* continuous kmalloc which can be used before the heap is initialised
* @param size The size of the memory to be allocated
* @param align Should the memory be aligned?
* @param paddr Should the allocated memory be given a physical address?
*/
void* kmalloc_c(uint32_t size, int align, uint32_t* paddr);

/*
* Wrapper for kmalloc_int with align = 1
* @param size The size of the memory to be allocated
*/
void* kmalloc_a(uint32_t size);

/*
* Wrapper for kmalloc_int with align = 0 and paddr = paddr
* @param size The size of the memory to be allocated
* @param paddr Should the allocated memory be given a physical address?
*/
uint32_t kmalloc_p(uint32_t size, uint32_t* paddr);

/*
* Wrapper for kmalloc_int with align = 1 and paddr = paddr
* @param size The size of the memory to be allocated
* @param paddr Should the allocated memory be given a physical address?
*/
uint32_t kmalloc_ap(uint32_t size, uint32_t* paddr);

/*
* Wrapper for kmalloc_int with align = 0 and paddr = 0
* @param size The size of the memory to be allocated
*/
void *kmalloc(uint32_t size);

/*
* A function to free a block of memory previously allocated with kmalloc
* @param ptr The pointer to the memory to be freed
*/
void kfree(void* ptr);

/*
* A function to reallocate a block of memory previously allocated with kmalloc
* @param ptr The pointer to the memory to be reallocated
* @param size The size of the memory to be reallocated
*/
void* krealloc(void* ptr, uint32_t size);

void* kcalloc(uint32_t size, uint32_t num);

void print_db();

int doesItFit(memory_block_t* block, uint32_t size);

void setFree(uint32_t *size, int x);

void removeFromList(memory_block_t* block);
void addToList(memory_block_t* block);

int isBetter(memory_block_t* block1, memory_block_t* block2);
memory_block_t* bestFit(uint32_t size);
memory_block_t* getPrevBlock(memory_block_t* block);
memory_block_t* getNextBlock(memory_block_t* block);

uint32_t getRealSize(uint32_t size);
uint32_t getSbrkSize(uint32_t size);

int isFree(memory_block_t* block);
int isEnd(memory_block_t* block);

void init_kheap(void* start, void* end, void* max);

void* malloc(uint32_t size);
void* realloc(void* ptr, uint32_t size);
void* calloc(uint32_t size, uint32_t num);

void free(void* ptr);
#endif