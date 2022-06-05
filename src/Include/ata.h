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

#define COMMAND_IDENTIFY_DEVICE 0xEC
#define COMMAND_DMA_READ        0xC8
#define COMMAND_READ_PIO        0x20

#define STATUS_ERR  0x00
#define STATUS_DRQ  0x08
#define STATUS_SRV  0x10
#define STATUS_DF   0x20
#define STATUS_RDY  0x40
#define STATUS_BSY  0x80

#define BMR_STATUS_DMA_ACTIVE 0x01
#define BMR_STATUS_DMA_ERROR  0x02
#define BMR_STATUS_DMA_IRQ    0x04

#define ATA_NOT_EQUALS 0x01
#define ATA_EQUALS     0x00

#define ERR_UNDETECTED 0x00
#define ERR_DETECTED 0x01

#define BMR_COMMAND_DMA_START   0x01
#define BMR_COMMAND_DMA_STOP    0x00
#define BMR_COMMAND_READ        0x08

#define BMR_STATUS_INT 0x04
#define BMR_STATUS_ERR 0x02

#define SECTOR_SIZE 512
#define MARK_END    0x8000

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