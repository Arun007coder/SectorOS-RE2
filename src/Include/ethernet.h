#ifndef __ETHERNET_H__
#define __ETHERNET_H__

#include "system.h"
#include "netinterface.h"
#include "netutils.h"
#include "kheap.h"
#include "arp.h"
#include "ipv4.h"

#define ETHERNET_TYPE_ARP 0x0806
#define ETHERNET_TYPE_IP  0x0800

typedef struct etherframe
{
    uint8_t dst_macaddr[6];
    uint8_t src_macaddr[6];
    uint16_t type;
}__attribute__((packed)) etherframe_t;

int ethernet_send(uint8_t *dst_macaddr, void* data, int size, uint16_t protocol);
void ethernet_handle_packet(etherframe_t* packet, int size);

#endif