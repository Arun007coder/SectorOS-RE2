#ifndef __ICMP_H__
#define __ICMP_H__

#include "system.h"
#include "ipv4.h"
#include "string.h"
#include "kheap.h"
#include "netutils.h"

#define ICMP_TYPE_ECHO_REPLY            0
#define ICMP_TYPE_DEST_UNREACHABLE      3
#define ICMP_TYPE_SOUCE_QUENCH          4
#define ICMP_TYPE_REDIRECT_MSG          5
#define ICMP_TYPE_ECHO_REQUEST          8
#define ICMP_TYPE_ROUTER_ADVERTISEMENT  9
#define ICMP_TYPE_ROUTER_SOLICITATION   10
#define ICMP_TYPE_TIME_EXCEEDED         11
#define ICMP_TYPE_BAD_PARAM             12
#define ICMP_TYPE_TIMESTAMP             13
#define ICMP_TYPE_TIMESTAMP_REPLY       14
#define ICMP_TYPE_INFO_REQUEST          15
#define ICMP_TYPE_INFO_REPLY            16
#define ICMP_TYPE_ADDR_MASK_REQUEST     17
#define ICMP_TYPE_ADDR_MASK_REPLY       18
#define ICMP_TYPE_TRACEROUTE            30

typedef struct icmp_packet
{
    uint8_t type;
    uint8_t code;
    uint16_t checksum;

    uint16_t id;
    uint16_t seq;
} __attribute__((packed)) icmp_packet_t;

void request_echo_reply(uint8_t *ip);

void icmp_handle_packet(icmp_packet_t* packet, uint8_t *srcip);

#endif