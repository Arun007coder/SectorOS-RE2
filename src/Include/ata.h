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

#ifndef __ATA_H__
#define __ATA_H__

#include "system.h"
#include "printf.h"
#include "string.h"
#include "port.h"
#include "vfs.h"
#include "devfs.h"
#include "pci.h"
#include "kheap.h"
#include "paging.h"
#include "isr.h"

#define ATA_PCI_VENDOR_ID 0x8086
#define ATA_PCI_DEVICE_ID 0x7010

#define PCI_BUS_MASTER_ENABLE 0x4

#define CONTROL_STOP_INTERRUPT  0x02
#define CONTROL_SOFT_RESET      0x04
#define CONTROL_HIGH_ORDER_BYTE 0x80
#define CONTROL_ZERO            0x00

#define COMMAND_READ_PIO          0x20
#define COMMAND_READ_PIO_EXT      0x24
#define COMMAND_DMA_READ          0xC8
#define COMMAND_DMA_READ_EXT      0x25
#define COMMAND_WRITE_PIO         0x30
#define COMMAND_WRITE_PIO_EXT     0x34
#define COMMAND_DMA_WRITE         0xCA
#define COMMAND_DMA_WRITE_EXT     0x35
#define COMMAND_CACHE_FLUSH       0xE7
#define COMMAND_CACHE_FLUSH_EXT   0xEA
#define COMMAND_PACKET            0xA0
#define COMMAND_IDENTIFY_PACKET   0xA1
#define COMMAND_IDENTIFY          0xEC

#define STATUS_ERR  0x00
#define STATUS_DRQ  0x08
#define STATUS_SRV  0x10
#define STATUS_DF   0x20
#define STATUS_RDY  0x40
#define STATUS_BSY  0x80

#define BMR_STATUS_DMA_ACTIVE 0x01
#define BMR_STATUS_DMA_ERROR  0x02
#define BMR_STATUS_DMA_IRQ    0x04

#define BMR_COMMAND_DMA_START   0x01
#define BMR_COMMAND_DMA_STOP    0x00
#define BMR_COMMAND_READ        0x08

#define BMR_STATUS_INT 0x04
#define BMR_STATUS_ERR 0x02

#define SECTOR_SIZE 512
#define MARK_END    0x8000

#define IDENT_DEVICETYPE   0
#define IDENT_CYLINDERS    2
#define IDENT_HEADS        6
#define IDENT_SECTORS      12
#define IDENT_SERIAL       20
#define IDENT_MODEL        54
#define IDENT_CAPABILITIES 98
#define IDENT_FIELDVALID   106
#define IDENT_MAX_LBA      120
#define IDENT_COMMANDSETS  164
#define IDENT_MAX_LBA_EXT  200

extern page_directory_t *kernel_page_dir;

typedef struct prdt
{
    uint32_t pbuffer;
    uint16_t transfer_size;
    uint16_t mark_end;
}__attribute__((packed)) prdt_t;

typedef struct ata_device
{
    uint16_t data;
	uint16_t error;
	uint16_t sector_count;

	union {
		uint16_t sector_num;
		uint16_t lba_lo ;
	};
	union {
		uint16_t cylinder_low;
		uint16_t lba_mid ;
	};
	union {
		uint16_t cylinder_high;
		uint16_t lba_high;
	};
	union {
		uint16_t drive;
		uint16_t head;
	};
	union {
		uint16_t command;
		uint16_t status;
	};
	union {
		uint16_t control;
		uint16_t alt_status;
	};

	int slave;
	int primary;
	uint32_t bar4;
	uint32_t BMR_COMMAND;
	uint32_t BMR_prdt;
	uint32_t BMR_STATUS;


	prdt_t * prdt;
	uint8_t * prdt_phys;

	uint8_t * mem_buffer;
	uint8_t * mem_buffer_phys;
}__attribute__((packed)) ata_dev_t;

void io_wait(ata_dev_t * dev);
void soft_reset(ata_dev_t * dev);

char* ata_read_sector(ata_dev_t * dev, uint32_t lba);
void ata_write_sector(ata_dev_t * dev, uint32_t lba, char * buffer);

void ata_device_init(ata_dev_t * dev, int primary);
void ata_device_detect(ata_dev_t * dev, int primary);

void init_ata();

#endif