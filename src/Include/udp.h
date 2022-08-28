#ifndef __UDP_H__
#define __UDP_H__

#include <system.h>
#include "ipv4.h"
#include <string.h>
#include "kheap.h"

#define MAX_SOCKETS 16

typedef void (*udp_callback_t)(ipv4_addr_t *addr, uint8_t *data, uint32_t len);

typedef struct udp_socket
{
    uint16_t port;
    udp_callback_t callback;
} udp_socket_t;

typedef struct udp_packet
{
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;
    uint16_t checksum;
    uint8_t data[];
}__attribute__((packed)) udp_packet_t;

void register_udp_callback(udp_callback_t callback, uint16_t port);

void udp_send_packet(uint8_t * dst_ip, uint16_t src_port, uint16_t dst_port, void * data, int len);

void udp_handle_packet(udp_packet_t * packet, ipv4_addr_t * src_addr);

void init_udp();

#endif