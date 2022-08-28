#ifndef __NETUTILS_H__
#define __NETUTILS_H__

#include "system.h"

typedef struct ip
{
    uint64_t ip0;
    uint64_t ip1;
    uint64_t ip2;
    uint64_t ip3;
}ip_t;

typedef struct mac
{
    uint64_t mac0;
    uint64_t mac1;
    uint64_t mac2;
    uint64_t mac3;
    uint64_t mac4;
    uint64_t mac5;
}mac_t;

uint16_t flip_short(uint16_t short_int);
uint32_t flip_long(uint32_t long_int);
uint8_t flip_byte(uint8_t byte, int num_bits);

uint8_t htonb(uint8_t byte, int num_bits);
uint16_t htons(uint16_t hostshort);
uint32_t htonl(uint32_t hostlong);

uint8_t ntohb(uint8_t byte, int num_bits);
uint16_t ntohs(uint16_t netshort);
uint32_t ntohl(uint32_t netlong);

void get_mac(uint8_t mac[6]);
ip_t get_ip();

void get_ip2(uint8_t* ip);
void set_ip(uint8_t* ip);

void iptoa(uint64_t ipaddr, uint8_t* ip);

uint16_t checksum(uint16_t * buffer, int size);

#endif