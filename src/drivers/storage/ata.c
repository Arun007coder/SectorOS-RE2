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

#include "ata.h"
#include "mbr.h"

pci_t ata_device;

ata_dev_t primary_master = {.slave = 0};
ata_dev_t primary_slave = {.slave = 1};
ata_dev_t secondary_master = {.slave = 0};
ata_dev_t secondary_slave = {.slave = 1};


void io_wait(ata_dev_t * dev)
{
    inb(dev->alt_status);
    inb(dev->alt_status);
    inb(dev->alt_status);
    inb(dev->alt_status);
}

void soft_reset(ata_dev_t * dev)
{
    outb(dev->control, CONTROL_SOFT_RESET);
    io_wait(dev);
    outb(dev->control, CONTROL_ZERO);
}

void ata_handler(registers_t* reg)
{
    inb(primary_master.status);
    inb(primary_master.BMR_STATUS);
    outb(primary_master.BMR_COMMAND, BMR_COMMAND_DMA_STOP);

    inb(primary_slave.status);
    inb(primary_slave.BMR_STATUS);
    outb(primary_slave.BMR_COMMAND, BMR_COMMAND_DMA_STOP);
}

void ata_shandler(registers_t *reg)
{
    inb(secondary_master.status);
    inb(secondary_master.BMR_STATUS);
    outb(secondary_master.BMR_COMMAND, BMR_COMMAND_DMA_STOP);

    inb(secondary_slave.status);
    inb(secondary_slave.BMR_STATUS);
    outb(secondary_slave.BMR_COMMAND, BMR_COMMAND_DMA_STOP);
}

void ata_open(vfs_node * node, uint32_t flags)
{
    return;
}

void ata_close(vfs_node * node)
{
    return;
}

void ata_stopInterrupts(ata_dev_t * dev)
{
    outb(dev->control, CONTROL_ZERO);
}

uint32_t ata_read(vfs_node * node, uint32_t offset, uint32_t size, char * buf)
{
    uint32_t start = offset / SECTOR_SIZE;
    uint32_t start_offset = offset % SECTOR_SIZE;

    uint32_t end = (offset + size - 1) / SECTOR_SIZE;
    uint32_t end_offset = (offset + size - 1) % SECTOR_SIZE;

    char * buf_curr = buf;
    uint32_t counter = start;
    uint32_t read_size;
    uint32_t off, total = 0;

    while(counter <= end)
    {
        off = 0;
        read_size = SECTOR_SIZE;

        char * ret = ata_read_sector((ata_dev_t*)node->device, counter);

        if(counter == start)
        {
            off = start_offset;
            read_size = SECTOR_SIZE - off;
        }
        if(counter == end)
            read_size = end_offset - off + 1;

        memcpy(buf_curr, ret + off, read_size);
        buf_curr = buf_curr + read_size;
        total = total + read_size;
        counter++;
    }
    return total;
}

uint32_t ata_write(vfs_node * node, uint32_t offset, uint32_t size, char * buf)
{
    uint32_t start = offset / SECTOR_SIZE;
    uint32_t start_offset = offset % SECTOR_SIZE;

    uint32_t end = (offset + size - 1) / SECTOR_SIZE;
    uint32_t end_offset = (offset + size - 1) % SECTOR_SIZE;

    char * buf_curr = buf;
    uint32_t counter = start;
    uint32_t write_size;
    uint32_t off, total = 0;

    while(counter <= end)
    {
        off = 0;
        write_size = SECTOR_SIZE;
        char * ret = ata_read_sector((ata_dev_t*)node->device, counter);
        if(counter == start)
        {
            off = start_offset;
            write_size = SECTOR_SIZE - off;
        }
        if(counter == end)
        {
            write_size = end_offset - off + 1;
        }
        memcpy(ret + off, buf_curr, write_size);
        ata_write_sector((ata_dev_t*)node->device, counter, ret);
        buf_curr = buf_curr + write_size;
        total = total + write_size;
        counter++;
    }
    return total;
}

void ata_write_sector(ata_dev_t * dev, uint32_t lba, char * buf)
{
    memcpy(dev->mem_buffer, buf, SECTOR_SIZE);

    outb(dev->BMR_COMMAND, 0);
    outl(dev->BMR_prdt, (uint32_t)dev->prdt_phys);

    outb(dev->drive, 0xE0 | dev->slave << 4 | (lba & 0x0f000000) >> 24);

    outb(dev->sector_count, 1);
    outb(dev->lba_lo, lba & 0x000000ff);
    outb(dev->lba_mid, (lba & 0x0000ff00) >> 8);
    outb(dev->lba_high, (lba & 0x00ff0000) >> 16);

    outb(dev->command, 0xCA);

    outb(dev->BMR_COMMAND, 0x1);

    while (1)
    {
        int status = inb(dev->BMR_STATUS);
        int dstatus = inb(dev->status);
        if (!(status & 0x04))
        {
            continue;
        }
        if (!(dstatus & 0x80))
        {
            break;
        }
    }
}

char * ata_read_sector(ata_dev_t * dev, uint32_t lba)
{
    while((inb(dev->status) & STATUS_BSY));

    char * buf = kmalloc(SECTOR_SIZE);

    outb(dev->BMR_COMMAND, 0);
    outl(dev->BMR_prdt, (uint32_t)dev->prdt_phys);

    outb(dev->drive, 0xE0 | dev->slave << 4 | (lba & 0x0f000000) >> 24);

    outb(dev->sector_count, 1);
    outb(dev->lba_lo, lba & 0x000000ff);
    outb(dev->lba_mid, (lba & 0x0000ff00) >> 8);
    outb(dev->lba_high, (lba & 0x00ff0000) >> 16);

    outb(dev->command, 0xC8);

    while(!(inb(dev->status) & STATUS_DRQ));

    outb(dev->BMR_COMMAND, 0x8 | 0x1);

    while (1)
    {
        int status = inb(dev->BMR_STATUS);
        int dstatus = inb(dev->status);
        if (!(status & 0x04))
        {
            continue;
        }
        if (!(dstatus & 0x80))
        {
            break;
        }
    }
    memcpy(buf, dev->mem_buffer, SECTOR_SIZE);
    return buf;
}

vfs_node * create_ata_device(ata_dev_t * dev)
{
    vfs_node * node = kcalloc(sizeof(vfs_node), 1);
    memset(node->name, 0, 256);
    strcpy(node->name, "ata");
    strcat(node->name, ((dev->data == 0x1F0) ? (dev->slave ? "1" : "0") : (dev->slave ? "3" : "2")));
    node->device = dev;
    node->flags = FS_BLOCKDEVICE;
    node->read = ata_read;
    node->write = ata_write;
    node->open = ata_open;
    node->close = ata_close;
    return node;
}

void ata_device_init(ata_dev_t * dev, int primary)
{
    dev->prdt = (void*)kmalloc_a(sizeof(prdt_t));
    memset(dev->prdt, 0, sizeof(prdt_t));
    dev->prdt_phys = virt2phys(kernel_page_dir, dev->prdt);
    dev->mem_buffer = (void*)kmalloc_a(4096);
    memset(dev->mem_buffer, 0, 4096);

    dev->prdt[0].pbuffer = (uint32_t)virt2phys(kernel_page_dir, dev->mem_buffer);
    dev->prdt[0].transfer_size = SECTOR_SIZE;
    dev->prdt[0].mark_end = MARK_END;

    uint16_t base_addr = primary ? (0x1F0) : (0x170);
    uint16_t alt_status = primary ? (0x3F6) : (0x376);

    dev->data = base_addr;
    dev->error = base_addr + 1;
    dev->sector_count = base_addr + 2;
    dev->lba_lo = base_addr + 3;
    dev->lba_mid = base_addr + 4;
    dev->lba_high = base_addr + 5;
    dev->drive = base_addr + 6;
    dev->command = base_addr + 7;
    dev->alt_status = alt_status;


    dev->bar4 = pci_read(ata_device, PCI_OFF_BAR4);
    if(dev->bar4 & 0x1)
    {
        dev->bar4 = dev->bar4 & 0xfffffffc;
    }
    dev->BMR_COMMAND = dev->bar4;
    dev->BMR_STATUS = dev->bar4 + 2;
    dev->BMR_prdt = dev->bar4 + 4;
}

void ata_device_detect(ata_dev_t * dev, int primary)
{
    ata_device_init(dev, primary);

    soft_reset(dev);
    io_wait(dev);

    outb(dev->drive, (0xA + dev->slave) << 4);

    outb(dev->sector_count, 0);
    outb(dev->lba_lo, 0);
    outb(dev->lba_mid, 0);
    outb(dev->lba_high, 0);

    outb(dev->command, COMMAND_IDENTIFY);

    if(!inb(dev->status))
    {
        printE("[ATA] Device does not exist\n");
        return;
    }

    uint8_t lba_lo = inb(dev->lba_lo);
    uint8_t lba_hi = inb(dev->lba_high);

    if(lba_lo != 0 || lba_hi != 0)
    {
        printE("[ATA] Not an ATA device\n");
        return;
    }

    uint8_t drq = 0, err = 0;
    uint32_t i = 0;
    while(!drq && !err)
    {
        drq = inb(dev->status) & STATUS_DRQ;
        err = inb(dev->status) & STATUS_ERR;
        if(i > 100)
        {
            printE("[ATA] Timeout\n");
            return;
        }
        i++;
    }

    if(err)
    {
        printE("[ATA] Error occurred when polling\n");
        return;
    }

    printf("[ATA] Device found\n");

    uint16_t * id = (uint16_t*)kmalloc(256);
    memset(id, 0, 256);
    uint8_t * id_buf = (uint8_t*)id;

    for(i = 0; i < 512; i++)
    {
        id[i] = inw(dev->data);
    }

    char* model = (char*)kmalloc(40);

    for(i = 0; i < 40; i += 2)
    {
        model[i] = id_buf[54 + i + 1];
        model[i + 1] = id_buf[54 + i];
    }

    printf("[ATA] Model: %s\n", model);

    uint32_t pci_command_reg = pci_read(ata_device, PCI_OFF_COMMAND);
    if(!(pci_command_reg & (1 << 2)))
    {
        pci_command_reg |= (1 << 2);
        pci_write(ata_device, PCI_OFF_COMMAND, pci_command_reg);
    }

    FILE* f = create_ata_device(dev);

    devfs_add(f);
    char* name = kmalloc(strlen(f->name) + 20);
    sprintf(name, "/dev/%s", f->name);

    probe_partitions(name);
    kfree(name);
}

void init_ata()
{
    if(pci_isDeviceAvailable(ATA_PCI_VENDOR_ID, ATA_PCI_DEVICE_ID) == 0) // Not found
    {
        printf("[ATA] Device not found\n");
        return;
    }

    ata_device = pci_get_device(ATA_PCI_VENDOR_ID, ATA_PCI_DEVICE_ID, -1);

    register_interrupt_handler(IRQ(14), ata_handler);
    register_interrupt_handler(IRQ(15), ata_shandler);

    ata_device_detect(&primary_master, 1);
    ata_device_detect(&primary_slave, 1);
    ata_device_detect(&secondary_master, 0);
    ata_device_detect(&secondary_slave, 0);

    printf("[ATA] ATA device initialized\n");
}