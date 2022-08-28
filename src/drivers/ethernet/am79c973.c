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

#include "am79c973.h"
#include "netinterface.h"
#include "ethernet.h"

static AM79C_dev_t* dev;

int isAM79CInit = 0;

void AM97C_ISR(registers_t *r)
{
    outw(dev->registerAddrPort, 0);
    uint32_t temp = inw(dev->registerDataPort);

    if((temp & AM79C_EUNKNOWN) == AM79C_EUNKNOWN)
    {
        printE("AM79C973: Unknown error\n");
    }
    if((temp & AM79C_ECOLLISION) == AM79C_ECOLLISION)
    {
        printE("AM79C973: Collision error\n");
    }
    if((temp & AM79C_EFRAMEMISS) == AM79C_EFRAMEMISS)
    {
        printE("AM79C973: Missed frame\n");
    }
    if((temp & AM79C_EMEMORY) == AM79C_EMEMORY)
    {
        printE("AM79C973: Memory error\n");
    }
    if((temp & AM79C_RECV) == AM79C_RECV)
    {
        AM79C_receive();
    }
    if((temp & AM79C_SENT) == AM79C_SENT)
    {
        printf(" AM97C973: SENT ");
    }

    outw(dev->registerAddrPort, 0);
    outw(dev->registerDataPort, temp);

    if((temp & AM79C_INIT) == AM79C_INIT)
    {
        printf("AM79C973: Surcessfully initialised\n");
    }
}

void init_AM79C()
{
    asm("cli");

    if(pci_isDeviceAvailable(AM79C_VENDORID, AM79C_DEVICEID) == 0) // Not Found
    {
        printf("[AM79C973] Device did not found\n");
        asm("sti");
        return;
    }

    if(isAM79CInit == 0)
    {
        pci_t pci_dev = pci_get_device(AM79C_VENDORID, AM79C_DEVICEID, -1);

        dev = (AM79C_dev_t*)kmalloc(sizeof(AM79C_dev_t));
        memset(dev, 0, sizeof(AM79C_dev_t));

        for(int i = 0; i < 6; i++)
        {
            uint32_t bar_val = pci_read(pci_dev, 0x10 + 4*i);
            if(BAR_IO_GETADDR(bar_val) && (BAR_TYPE(bar_val) == PCI_DEVTYPE_IO))
            {
                dev->portBase = (uint32_t)BAR_IO_GETADDR(bar_val);
                break;
            }
        }

        dev->MACAddressPort0    = dev->portBase;
        dev->MACAddressPort2    = (dev->portBase + 0x02);
        dev->MACAddressPort4    = (dev->portBase + 0x04);
        dev->registerDataPort   = (dev->portBase + 0x10);
        dev->registerAddrPort   = (dev->portBase + 0x12);
        dev->resetPort          = (dev->portBase + 0x14);
        dev->busCTRLRegisterData= (dev->portBase + 0x16);

        dev->interrupt = pci_read(pci_dev, PCI_OFF_INTERRUPT_LINE);

        dev->currentSendBuffer = 0;
        dev->currentRecvBuffer = 0;

        uint64_t MAC0 = inw(dev->MACAddressPort0) % 256;
        uint64_t MAC1 = inw(dev->MACAddressPort0) / 256;
        uint64_t MAC2 = inw(dev->MACAddressPort2) % 256;
        uint64_t MAC3 = inw(dev->MACAddressPort2) / 256;
        uint64_t MAC4 = inw(dev->MACAddressPort4) % 256;
        uint64_t MAC5 = inw(dev->MACAddressPort4) / 256;

        uint64_t MAC = MAC5 << 40 | MAC4 << 32 | MAC3 << 24 | MAC2 << 16 | MAC1 << 8 | MAC0;

        outw(dev->registerAddrPort, 0x14);
        outw(dev->busCTRLRegisterData, 0x102);

        outw(dev->registerAddrPort, 0);
        outw(dev->registerDataPort, 0x04);

        dev->initblock.mode             = 0x0000;
        dev->initblock.reserved1        = 0;
        dev->initblock.numSendBuffers   = 3;
        dev->initblock.reserved2        = 0;
        dev->initblock.numRecBuffers    = 3;
        dev->initblock.physicalAddress  = MAC;
        dev->initblock.reserved3        = 0;
        dev->initblock.logical_address  = 0;

        dev->sendBufferDescr = (AM79C_BD_t*)((((uint32_t)&dev->sendBufferDescrMem[0]) + 15) & ~((uint32_t)0x0F));
        dev->initblock.SendBufferDescrAddress = (uint32_t)dev->sendBufferDescr;
        dev->recvBufferDescr = (AM79C_BD_t*)((((uint32_t)&dev->recvBufferDescrMemory[0]) + 15) & ~((uint32_t)0x0F));
        dev->initblock.RecBufferDescrAddress = (uint32_t)dev->recvBufferDescr;

        for (uint8_t i = 0; i < 8; i++)
        {
            dev->sendBufferDescr[i].address = (((uint32_t)&dev->sendBuffers[i]) + 15) & ~(uint32_t)0x0F;
            dev->sendBufferDescr[i].flags = 0x7FF | 0xF000;
            dev->sendBufferDescr[i].flags2 = 0;
            dev->sendBufferDescr[i].avail = 0;

            dev->recvBufferDescr[i].address = (((uint32_t)&dev->recvBuffers[i]) + 15) & ~(uint32_t)0x0F;
            dev->recvBufferDescr[i].flags = 0xF7FF | 0x80000000;
            dev->recvBufferDescr[i].flags2 = 0;
            dev->sendBufferDescr[i].avail = 0;
        }

        outw(dev->registerAddrPort, 1);
        outw(dev->registerDataPort, (uint32_t)(&dev->initblock) & 0xFFFF);
        outw(dev->registerAddrPort, 2);
        outw(dev->registerDataPort, ((uint32_t)(&dev->initblock) >> 16) & 0xFFFF);

        outw(dev->registerAddrPort, 0);
        outw(dev->registerDataPort, 0x41);

        outw(dev->registerAddrPort, 4);
        uint32_t temp = inw(dev->registerDataPort);
        outw(dev->registerAddrPort, 4);
        outw(dev->registerDataPort, (temp | 0xC00));

        outw(dev->registerAddrPort, 0);
        outw(dev->registerDataPort, 0x42);

        register_interrupt_handler(0x20 + dev->interrupt, AM97C_ISR);

        net_t* interface = kcalloc(sizeof(net_t), 1);
        interface->IUID = AM79C973_IUID;
        interface->send = AM79C_send;
        interface->getip = AM97C_GetIPAddress;
        interface->getmac = AM97C_GetMACAddr;
        interface->setip = AM97C_SetIPAddress;

        register_network_interface(interface);

        printf("AM79C973: Activated\n");

        isAM79CInit = 1;
        asm("sti");
    }
    else
    {
        return;
    }
}

void AM79C_reset()
{
    inw(dev->resetPort);
    outw(dev->resetPort, 0x42);

    printf("AM79C973: Reset\n");
}

void AM79C_send(void* buffer, uint32_t count)
{
    int sendDescriptor = dev->currentSendBuffer;

    dev->currentSendBuffer = (dev->currentSendBuffer + 1) % 8;

    if(count > 1518)
        count = 1518;

    for(uint8_t *src = buffer + count - 1, *dst = (uint8_t *)(dev->sendBufferDescr[sendDescriptor].address + count - 1); src >= buffer; src--, dst--)
    {
        *dst = *src;
    }

    dev->sendBufferDescr[sendDescriptor].avail = 0;
    dev->sendBufferDescr[sendDescriptor].flags2 = 0;
    dev->sendBufferDescr[sendDescriptor].flags = 0x8300F000 | ((uint16_t)((-count) & 0xFFF));
    outw(dev->registerAddrPort, 0);
    outw(dev->registerDataPort, 0x48);
}

void AM79C_receive()
{

    printf("AM79C973: Receiving data\n");
    for (; (dev->recvBufferDescr[dev->currentRecvBuffer].flags & 0x80000000) == 0; dev->currentRecvBuffer = (dev->currentRecvBuffer + 1) % 8)
    {
        if (!(dev->recvBufferDescr[dev->currentRecvBuffer].flags & 0x40000000) && (dev->recvBufferDescr[dev->currentRecvBuffer].flags & 0x03000000) == 0x03000000)
        {
            uint32_t size = dev->recvBufferDescr[dev->currentRecvBuffer].flags & 0xFFF;
            if (size > 64)
                size -= 4;

            uint8_t *buffer = (uint8_t *)(dev->recvBufferDescr[dev->currentRecvBuffer].address);

            for (int i = 14 + 20; i < (size > 64 ? 64 : size); i++)
            {
                printf("%02x ", buffer[i]);
            }
            printf("\n");

            etherframe_t* packet = (etherframe_t*)buffer;

            interface_receive(packet, size);
        }

        dev->recvBufferDescr[dev->currentRecvBuffer].flags2 = 0;
        dev->recvBufferDescr[dev->currentRecvBuffer].flags = 0x8000F7FF;
    }
}

uint64_t AM97C_GetMACAddr()
{
    return dev->initblock.physicalAddress;
}

uint32_t AM97C_GetIPAddress()
{
    return dev->initblock.logical_address;
}

void AM97C_SetIPAddress(uint32_t ip)
{
    dev->initblock.logical_address = ip;
}