#ifndef __RTL8139_H__
#define __RTL8139_H__

#include "system.h"
#include "kheap.h"
#include "mmio.h"
#include "paging.h"
#include "netutils.h"
#include "netinterface.h"
#include "pci.h"
#include "string.h"
#include "port.h"
#include "ethernet.h"

#define RTL8139_VENDOR_ID 0x10EC
#define RTL8139_DEVICE_ID 0x8139

#define RX_BUF_SIZE 0x2000

#define CAPR 0x38
#define RX_READ_POINTER_MASK (~3)
#define ROK                 (1<<0)
#define RER                 (1<<1)
#define TOK                 (1<<2)
#define TER                 (1<<3)
#define TX_TOK              (1<<15)

enum RTL8139_registers
{
    MAG0             = 0x00,
    MAR0             = 0x08,
    TxStatus0        = 0x10,
    TxAddr0          = 0x20,
    RxBuf            = 0x30,
    RxEarlyCnt       = 0x34,
    RxEarlyStatus    = 0x36,
    ChipCmd          = 0x37,
    RxBufPtr         = 0x38,
    RxBufAddr        = 0x3A,
    IntrMask         = 0x3C,
    IntrStatus       = 0x3E,
    TxConfig         = 0x40,
    RxConfig         = 0x44,
    Timer            = 0x48,
    RxMissed         = 0x4C,
    Cfg9346          = 0x50,
    Config0          = 0x51,
    Config1          = 0x52,
    FlashReg         = 0x54,
    GPPinData        = 0x58,
    GPPinDir         = 0x59,
    MII_SMI          = 0x5A,
    HltClk           = 0x5B,
    MultiIntr        = 0x5C,
    TxSummary        = 0x60,
    MII_BMCR         = 0x62,
    MII_BMSR         = 0x64,
    NWayAdvert       = 0x66,
    NWayLPAR         = 0x68,
    NWayExpansion    = 0x6A,

    FIFOTMS          = 0x70,
    CSCR             = 0x74,
    PARA78           = 0x78,
    PARA7c           = 0x7c,
};

typedef struct tx_desc
{
    uint32_t paddr;
    uint32_t packet_size;
} tx_desc_t;

typedef struct rtl8139_dev
{
    uint8_t bar_type;
    uint16_t io_base;
    uint32_t mem_base;
    int eeprom_exist;
    uint8_t mac_addr[6];
    uint8_t ip_addr[4];
    char* rx_buf;
    int tx_cur;
}rtl8139_dev_t;

void rtl8139_send(void * data, uint32_t len);

void rtl8139_handler(registers_t * reg);

void init_rtl8139();

void read_mac_addr();

void rtl8139_receive();

uint64_t rtl8139_get_mac_addr();
uint64_t rtl8139_get_ip_addr();

void rtl8139_set_ip_addr(uint64_t ip);

void rtl8139_reset();

#endif