#ifndef __AM79C973_H_
#define __AM79C973_H_

#include "system.h"
#include "port.h"
#include "printf.h"
#include "string.h"
#include "kheap.h"
#include "xxd.h"
#include "pci.h"

#define AM79C_VENDORID 0x1022
#define AM79C_DEVICEID 0x2000

#define AM79C_EUNKNOWN     0x8000
#define AM79C_ECOLLISION   0x2000
#define AM79C_EFRAMEMISS   0x1000
#define AM79C_EMEMORY      0x0800
#define AM79C_RECV         0x0400
#define AM79C_SENT         0x0200
#define AM79C_INIT         0x0100

#define U64_TO_MAC(x)   uint64_t MAC0 = (x >> 40) & 0xFF; \
                        uint64_t MAC1 = (x >> 32) & 0xFF; \
                        uint64_t MAC2 = (x >> 24) & 0xFF; \
                        uint64_t MAC3 = (x >> 16) & 0xFF; \
                        uint64_t MAC4 = (x >> 8)  & 0xFF; \
                        uint64_t MAC5 = (x & 0xFF);

typedef bool (*AM79C_OnReceive_handler_t)(uint8_t*, uint32_t);

typedef struct AM79C_INITBLOCK
{
    uint16_t mode;
    unsigned reserved1          : 4;
    unsigned numSendBuffers     : 4;
    unsigned reserved2          : 4;
    unsigned numRecBuffers      : 4;
    uint64_t physicalAddress    : 48; // MAC Address
    uint16_t reserved3;
    uint64_t logical_address;   // IP Address
    uint32_t RecBufferDescrAddress;
    uint32_t SendBufferDescrAddress;
}__attribute__((packed)) AM79C_INITBLOCK_t;

typedef struct AM79C_BUFFERDESCRIPTOR
{
    uint32_t address;
    uint32_t flags;
    uint32_t flags2;
    uint32_t avail;
}__attribute__((packed)) AM79C_BD_t;

typedef struct AM79C_dev
{
    uint32_t interrupt;

    // AMD pcnet fast III uses 16-bit ports
    uint32_t portBase;
    uint32_t MACAddressPort0;
    uint32_t MACAddressPort2;
    uint32_t MACAddressPort4;
    uint32_t registerDataPort;
    uint32_t registerAddrPort;
    uint32_t resetPort;  
    uint32_t busCTRLRegisterData;

    AM79C_INITBLOCK_t initblock;

    AM79C_BD_t* sendBufferDescr;
    uint8_t sendBufferDescrMem[2048  + 15];
    uint8_t sendBuffers[2 * 1024 + 15][8];
    uint8_t currentSendBuffer;

    AM79C_BD_t* recvBufferDescr;
    uint8_t recvBufferDescrMemory[2048 + 15];
    uint8_t recvBuffers[2 * 1024 + 15][8];
    uint8_t currentRecvBuffer;

    AM79C_OnReceive_handler_t onReciveHandler;

}AM79C_dev_t;

void init_AM79C();

void AM79C_send(uint8_t* buffer, int count);
void AM97C_receive();

int AM97C_reset();

uint64_t AM97C_GetMACAddr();

void AM97C_SetIPAddress(uint32_t ip);
uint32_t AM97C_GetIPAddress();

void AM97C_SetHandler(AM79C_OnReceive_handler_t onrec);

#endif