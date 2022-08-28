#ifndef __IP_H__
#define __IP_H__

#include "ethernet.h"
#include "system.h"
#include "string.h"
#include "kheap.h"
#include "printf.h"
#include "arp.h"

#define IP_VERSION 4

#define IP_PACKET_NO_FRAGMENT 2
#define IP_IS_LAST_FRAGMENT 4

#define PROTOCOL_ICMP 1
#define PROTOCOL_IGMP 2
#define PROTOCOL_TCP 6
#define PROTOCOL_UDP 17

typedef struct ipv4_addr
{
    uint8_t addr[4];
} ipv4_addr_t;

typedef struct ip_packet
{
    char version_ihl_ptr[0];
    uint8_t version : 4;
    uint8_t ihl : 4;
    uint8_t tos;
    uint16_t length;
    uint16_t id;
    char flags_fragment_ptr[0];
    uint8_t flags : 3;
    uint8_t fragment_offset_high : 5;
    uint8_t fragment_offset_low;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint8_t src_ip[4];
    uint8_t dst_ip[4];
    uint8_t data[];
}__attribute__((packed)) ip_packet_t;

char* ip_to_string(uint8_t* ip);
void get_ip_str(char* ip_str, uint8_t* ip);

uint16_t ip_checksum(ip_packet_t* packet);

void ip_send_packet(uint8_t* dstip, void* data, int len, uint8_t protocol);
void ip_handle_packet(ip_packet_t* packet);

#endif