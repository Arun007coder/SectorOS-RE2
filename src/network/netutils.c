#include "netutils.h"
#include "netinterface.h"

uint16_t flip_short(uint16_t short_int)
{
    uint32_t first_byte = *((uint8_t*)(&short_int));
    uint32_t second_byte = *((uint8_t*)(&short_int) + 1);
    return (first_byte << 8) | (second_byte);
}

uint32_t flip_long(uint32_t long_int)
{
    uint32_t first_byte = *((uint8_t*)(&long_int));
    uint32_t second_byte = *((uint8_t*)(&long_int) + 1);
    uint32_t third_byte = *((uint8_t*)(&long_int)  + 2);
    uint32_t fourth_byte = *((uint8_t*)(&long_int) + 3);
    return (first_byte << 24) | (second_byte << 16) | (third_byte << 8) | (fourth_byte);
}


uint8_t flip_byte(uint8_t byte, int num_bits)
{
    uint8_t t = byte << (8 - num_bits);
    return t | (byte >> num_bits);
}

uint8_t htonb(uint8_t byte, int num_bits)
{
    return flip_byte(byte, num_bits);
}

uint8_t ntohb(uint8_t byte, int num_bits)
{
    return flip_byte(byte, 8 - num_bits);
}

uint16_t htons(uint16_t hostshort)
{
    return flip_short(hostshort);
}

uint32_t htonl(uint32_t hostlong)
{
    return flip_long(hostlong);
}

uint16_t ntohs(uint16_t netshort)
{
    return flip_short(netshort);
}

uint32_t ntohl(uint32_t netlong)
{
    return flip_long(netlong);
}

void get_mac(uint8_t mac[6])
{
    uint64_t mac_addr = interface_getmac();
    for(int i = 0; i < 6; i++)
    {
        mac[i] = (uint8_t)(mac_addr >> (i * 8));
    }
}

ip_t get_ip()
{
    ip_t ip;
    uint64_t ipaddr = interface_getip();
    ip.ip0 = (ipaddr >> 24) & 0xFF;
    ip.ip1 = (ipaddr >> 16) & 0xFF;
    ip.ip2 = (ipaddr >> 8)  & 0xFF;
    ip.ip3 = ipaddr & 0xFF;

    return ip;
}

void iptoa(uint64_t ipaddr, uint8_t* ip)
{
    ip[0] = (ipaddr >> 24) & 0xFF;
    ip[1] = (ipaddr >> 16) & 0xFF;
    ip[2] = (ipaddr >> 8)  & 0xFF;
    ip[3] = ipaddr & 0xFF;
}

uint16_t checksum(uint16_t * buffer, int size)
{
    uint32_t sum = 0;
    for (int i = 0; i < size; i++)
    {
        sum += ntohs(buffer[i]);
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return (uint16_t)~sum;
}

void get_ip2(uint8_t* ip)
{
    uint64_t ipaddr = interface_getip();
    ip[0] = (ipaddr >> 24) & 0xFF;
    ip[1] = (ipaddr >> 16) & 0xFF;
    ip[2] = (ipaddr >> 8)  & 0xFF;
    ip[3] = ipaddr & 0xFF;
}

void set_ip(uint8_t ip[4])
{
    uint64_t ipaddr = (ip[0] << 24) | (ip[1] << 16) | (ip[2] << 8) | ip[3];
    interface_setip(ipaddr);
}
