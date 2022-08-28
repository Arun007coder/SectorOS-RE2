#ifndef __NETINTERFACE_H__
#define __NETINTERFACE_H__

#include "system.h"
#include "kheap.h"
#include "printf.h"
#include "list.h"

typedef void (*network_sent_t)(void*, uint32_t);

typedef uint64_t (*network_getip_t)();
typedef uint64_t (*network_getmac_t)();

typedef void (*network_setip_t)(uint64_t ip);

#define AM79C973_IUID 9262
#define RTL8139_IUID  8210

typedef struct net
{
    uint32_t IUID; // Interface Unique ID

    network_sent_t send;

    network_getip_t getip;
    network_getmac_t getmac;

    network_setip_t setip;
}net_t;

void register_network_interface(net_t* interface);

void init_networkInterfaceManager();

void switch_interface(uint32_t index);

void interface_send(uint8_t* buffer, int size);

uint64_t interface_getip();
uint64_t interface_getmac();

void interface_setip(uint64_t ip);

void interface_receive(uint8_t* packet, uint32_t size);

void changeNetworkState(bool state);

void print_interfaces();

#endif