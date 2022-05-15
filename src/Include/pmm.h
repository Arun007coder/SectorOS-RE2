#ifndef __PMM_H__
#define __PMM_H__

#include "system.h"
#include "printf.h"
#include "string.h"
#include "vga.h"

#define BLOCK_SIZE 4096
#define BPP 8

#define SETBIT(i) pmm_bitmap[i / BPP] = pmm_bitmap[i / BPP] | (1 << (i % BPP))
#define CLEARBIT(i) pmm_bitmap[i / BPP] = pmm_bitmap[i / BPP] & (~(1 << (i % BPP)))
#define ISSET(i) ((pmm_bitmap[i / BPP] >> (i % BPP)) & 0x1)
#define GET_BUCKET32(i) (*((uint32_t*) &pmm_bitmap[i / 32]))

#define BLOCK_ALIGN(i) (((i) & 0xFFFFF000) + 0x1000)

void init_pmm(uint32_t size);
uint32_t alloc_block();
void free_block(uint32_t block);
uint32_t first_free_block();

#endif