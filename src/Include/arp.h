#ifndef __ARP_H__
#define __ARP_H__

#include <system.h>
#include <kheap.h>
#include <ethernet.h>
#include <printf.h>
#include <string.h>

#define ARP_REQUEST 1
#define ARP_REPLY 2

#define HARDWARE_TYPE_ETHERNET 1

typedef struct arp_packet
{
    uint16_t hardwareType;
    uint16_t protocol;
    uint8_t hardwareAddressSize;
    uint8_t protocolAddressSize;
    uint16_t opcode;
    uint8_t srcMAC[6];
    uint8_t srcIP[4];
    uint8_t dstMAC[6];
    uint8_t dstIP[4];
} __attribute__((packed)) arp_packet_t;

typedef struct arp_table_entry
{
    uint32_t ip_addr;
    uint64_t mac_addr;
}arp_table_entry_t;

void arp_handle_packet(arp_packet_t *packet, int size);

void arp_send_packet(uint8_t* dst_hardware_addr, uint8_t* dst_protocol_addr);

int arp_lookup(uint8_t* ret_hardware_addr, uint8_t* ip_addr);
void arp_lookup_add(uint8_t* ret_hardware_addr, uint8_t* ip_addr);

void init_arp();

#endif