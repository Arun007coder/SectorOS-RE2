#include "ipv4.h"

#include "udp.h"
#include "icmp.h"

uint8_t my_ip[] = {10, 0, 2, 14};
uint8_t test_target_ip[] = {10, 0, 2, 15};
uint8_t zero_hardware_addr[] = {0,0,0,0,0,0};

void get_ip_str(char* ip_str, uint8_t* ip)
{
    sprintf(ip_str, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
}

char* ip_to_string(uint8_t* ip)
{
    static char ip_str[16];
    sprintf(ip_str, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    return ip_str;
}

uint16_t ip_checksum(ip_packet_t* packet)
{
    int array_size = sizeof(ip_packet_t) / 2;

    uint16_t * array = (uint16_t*)packet;
    uint8_t * array2 = (uint8_t*)packet;

    uint32_t sum = 0;

    for(int i = 0; i < array_size; i++)
    {
        sum += flip_short(array[i]);
    }

    uint32_t carry = sum >> 16;

    sum = sum & 0x0000ffff;
    sum = sum + carry;

    return ~sum;
}

void ip_send_packet(uint8_t* dst_ip, void* data, int len, uint8_t protocol)
{
    int arp_sent = 3;
    ip_packet_t * packet = kmalloc(sizeof(ip_packet_t) + len);
    memset(packet, 0, sizeof(ip_packet_t));

    packet->version = IP_VERSION;
    packet->ihl = 5;
    packet->tos = 0;
    packet->length = sizeof(ip_packet_t) + len;
    packet->id = 0;
    packet->flags = 0;
    packet->fragment_offset_high = 0;
    packet->fragment_offset_low = 0;
    packet->ttl = 64;
    packet->protocol = protocol;

    get_ip2(packet->src_ip);
    memcpy(packet->dst_ip, dst_ip, 4);

    void * packet_data = (void*)packet + packet->ihl * 4;
    memcpy(packet_data, data, len);

    *((uint8_t*)(&packet->version_ihl_ptr)) = htonb(*((uint8_t*)(&packet->version_ihl_ptr)), 4);
    *((uint8_t*)(packet->flags_fragment_ptr)) = htonb(*((uint8_t*)(packet->flags_fragment_ptr)), 3);
    packet->length = htons(sizeof(ip_packet_t) + len);

    packet->checksum = 0;
    packet->checksum = htons(ip_checksum(packet));

    uint8_t dst_hardware_addr[6];

    while(!arp_lookup(dst_hardware_addr, dst_ip))
    {
        if(arp_sent != 0)
        {
            arp_sent--;
            arp_send_packet(zero_hardware_addr, dst_ip);
        }
    }
    ethernet_send(dst_hardware_addr, packet, htons(packet->length), ETHERNET_TYPE_IP);
}

void ip_handle_packet(ip_packet_t* packet)
{
    *((uint8_t*)(&packet->version_ihl_ptr)) = ntohb(*((uint8_t*)(&packet->version_ihl_ptr)), 4);
    *((uint8_t*)(packet->flags_fragment_ptr)) = ntohb(*((uint8_t*)(packet->flags_fragment_ptr)), 3);

    char src_ip[20];

    if(packet->version == IP_VERSION)
    {
        ipv4_addr_t src_addr;
        src_addr.addr[0] = packet->src_ip[0];
        src_addr.addr[1] = packet->src_ip[1];
        src_addr.addr[2] = packet->src_ip[2];
        src_addr.addr[3] = packet->src_ip[3];

        void * data_ptr = (void*)packet + packet->ihl * 4;
        int data_len = ntohs(packet->length) - sizeof(ip_packet_t);

        switch(packet->protocol)
        {
            case PROTOCOL_ICMP:
                printf("ICMP packet\n");
                icmp_handle_packet(data_ptr, packet->src_ip);
                break;
            case PROTOCOL_TCP:
                printf("TCP packet\n");
                //tcp_handle_packet(packet);
                break;
            case PROTOCOL_UDP:
                printf("UDP packet\n");
                udp_handle_packet(data_ptr, &src_addr);
                break;
            default:
                printf("Unsupported protocol: %d\n", packet->protocol);
                break;
        }
    }
    else
    {
        printf("Unsupported IP version: %d\n", packet->version);
    }

    return;
}