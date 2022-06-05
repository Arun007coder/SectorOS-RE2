#ifndef __PCI_H__
#define __PCI_H__

#include "system.h"
#include "printf.h"
#include "port.h"
#include "serial.h"
#include "printf.h"

typedef union pci
{
    uint32_t bits;
    struct
    {
        uint32_t always_zero : 2;
        uint32_t field_num : 6;
        uint32_t function_num : 3;
        uint32_t device_num : 5;
        uint32_t bus_num : 8;
        uint32_t reserved : 7;
        uint32_t enable : 1;
    };
} pci_t;

#define PCI_CONFIG_PORT 0xCF8
#define PCI_DATA_PORT 0xCFC

#define PCI_OFF_VENDOR_ID 0x00
#define PCI_OFF_DEVICE_ID 0x02
#define PCI_OFF_COMMAND 0x04
#define PCI_OFF_STATUS 0x06
#define PCI_OFF_REVISION_ID 0x08
#define PCI_OFF_PROG_IF 0x09
#define PCI_OFF_SUBCLASS 0x0a
#define PCI_OFF_CLASS 0x0b
#define PCI_OFF_CACHE_LINE_SIZE 0x0c
#define PCI_OFF_LATENCY_TIMER 0x0d
#define PCI_OFF_HEADER_TYPE 0x0e
#define PCI_OFF_BIST 0x0f
#define PCI_OFF_BAR0 0x10
#define PCI_OFF_BAR1 0x14
#define PCI_OFF_BAR2 0x18
#define PCI_OFF_BAR3 0x1C
#define PCI_OFF_BAR4 0x20
#define PCI_OFF_BAR5 0x24
#define PCI_OFF_BAR(x) PCI_OFF_BAR##x
#define PCI_OFF_INTERRUPT_LINE 0x3C
#define PCI_OFF_SECONDARY_BUS 0x09

#define PCI_HEADER_TYPE_DEVICE 0
#define PCI_HEADER_TYPE_BRIDGE 1
#define PCI_HEADER_TYPE_CARDBUS 2
#define PCI_TYPE_BRIDGE 0x0604
#define PCI_TYPE_SATA 0x0106
#define PCI_NONE 0xFFFF

#define BUS_PER_DOMAIN 8
#define DEV_PER_BUS 32
#define FUNC_PER_DEV 32

#define PCI_BUS_MASTER 0x04

#define PCI_DEVTYPE_IO 1
#define PCI_DEVTYPE_MMIO 0

#define BAR_TYPE(x) (x & 0x01) ? PCI_DEVTYPE_IO : PCI_DEVTYPE_MMIO

#define BAR_IO_GETADDR(x) (uint8_t*)(x & ~0x03)

void init_pci();

uint32_t pci_read(pci_t dev, uint32_t field);
void pci_write(pci_t dev, uint32_t field, uint32_t value);

uint32_t get_device_type(pci_t dev);
uint32_t get_secondary_bus(pci_t dev);

uint32_t pci_reach_end(pci_t dev);

pci_t pci_scan_function(uint16_t vendor_id, uint16_t device_id, uint32_t bus, uint32_t device, uint32_t function, int device_type);
pci_t pci_scan_device(uint16_t vendor_id, uint16_t device_id, uint32_t bus, uint32_t device, int device_type);
pci_t pci_scan_bus(uint16_t vendor_id, uint16_t device_id, uint32_t bus, int device_type);
pci_t pci_get_device(uint16_t vendor_id, uint16_t device_id, int device_type);
void enumerate_pci_devices();
void procfs_enumerate_pci_devices(char* buffer);

#endif