#include "arp.h"

arp_table_entry_t arp_table[512];
int arp_table_size = 0;
int arp_table_current = 0;

uint8_t broadcast_mac_addr[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

void arp_handle_packet(arp_packet_t* packet, int len)
{
    char dstMAC[6];
    char dstIP[4];

    char srcMAC[6];

    memcpy(dstMAC, packet->dstMAC, 6);
    memcpy(dstIP, packet->dstIP, 4);
    memcpy(srcMAC, packet->srcMAC, 6);

    if(ntohs(packet->opcode) == ARP_REQUEST)
    {
        uint32_t my_ip = interface_getip();
        uint32_t dstip = *(packet->dstIP);
        printf("[ARP] ARP Request from %d.%d.%d.%d for %d.%d.%d.%d\n", packet->srcIP[0], packet->srcIP[1], packet->srcIP[2], packet->srcIP[3], packet->dstIP[0], packet->dstIP[1], packet->dstIP[2], packet->dstIP[3]);
        if(my_ip == dstip)
        {
            printf("[ARP] Replying to %02x:%02x:%02x:%02x:%02x:%02x\n", packet->srcMAC[0], packet->srcMAC[1], packet->srcMAC[2], packet->srcMAC[3], packet->srcMAC[4], packet->srcMAC[5]);
            get_mac(packet->srcMAC);
            get_ip2(packet->srcIP);

            memcpy(packet->dstMAC, dstMAC, 6);
            memcpy(packet->dstIP, dstIP, 4);

            packet->opcode = htons(ARP_REPLY);

            packet->hardwareAddressSize = 6;
            packet->protocolAddressSize = 4;

            packet->hardwareType = htons(HARDWARE_TYPE_ETHERNET);

            packet->protocol = htons(ETHERNET_TYPE_IP);

            ethernet_send(srcMAC, packet, sizeof(arp_packet_t), ETHERNET_TYPE_ARP);
        }
    }
    else if(ntohs(packet->opcode) == ARP_REPLY)
    {
        printf("[ARP] Got reply from %02x:%02x:%02x:%02x:%02x:%02x\n", packet->srcMAC[0], packet->srcMAC[1], packet->srcMAC[2], packet->srcMAC[3], packet->srcMAC[4], packet->srcMAC[5]);
        memcpy(&arp_table[arp_table_current].mac_addr, packet->srcMAC, 6);
        memcpy(&arp_table[arp_table_current].ip_addr, packet->srcIP, 4);
        arp_table_current++;

    }
    else 
    {
        printE("Unknown ARP opcode: %d\n", ntohs(packet->opcode));
    }

    if(arp_table_size < 512)
    {
        arp_table_size++;
    }

    if(arp_table_current >= 512)
    {
        arp_table_current = 0;
    }
}

void arp_send_packet(uint8_t* dstMAC, uint8_t* dstIP)
{
    arp_packet_t* packet = (arp_packet_t*)kmalloc(sizeof(arp_packet_t));

    get_mac(packet->srcMAC);
    get_ip2(packet->srcIP);

    memcpy(packet->dstMAC, dstMAC, 6);
    memcpy(packet->dstIP, dstIP, 4);
    packet->opcode = htons(ARP_REQUEST);

    packet->hardwareAddressSize = 6;
    packet->protocolAddressSize = 4;

    packet->hardwareType = htons(HARDWARE_TYPE_ETHERNET);

    packet->protocol = htons(ETHERNET_TYPE_IP);

    ethernet_send(broadcast_mac_addr, packet, sizeof(arp_packet_t), ETHERNET_TYPE_ARP);
}

void arp_lookup_add(uint8_t* ret_hardware_addr, uint8_t* ip_addr)
{
    memcpy(&arp_table[arp_table_current].mac_addr, ret_hardware_addr, 6);
    memcpy(&arp_table[arp_table_current].ip_addr, ip_addr, 4);
    arp_table_current++;

    if(arp_table_size < 512)
    {
        arp_table_size++;
    }

    if(arp_table_current >= 512)
    {
        arp_table_current = 0;
    }
}

int arp_lookup(uint8_t* ret_hardware_addr, uint8_t* ip_addr)
{
    uint32_t ip_entry = *((uint32_t*)(ip_addr));
    for(int i = 0; i < 512; i++)
    {
        if(arp_table[i].ip_addr == ip_entry)
        {
            memcpy(ret_hardware_addr, &arp_table[i].mac_addr, 6);
            return 1;
        }
    }
    return 0;
}

void init_arp()
{
    uint8_t broadcast_ip[4];
    uint8_t broadcast_mac[6];

    uint8_t ret_mac[6];

    memset(broadcast_ip, 0xff, 4);
    memset(broadcast_mac, 0xff, 6);
    arp_lookup_add(broadcast_mac, broadcast_ip);

    arp_lookup(ret_mac, broadcast_ip);

    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", ret_mac[0], ret_mac[1], ret_mac[2], ret_mac[3], ret_mac[4], ret_mac[5]);

    ASSERT(memcmp(ret_mac, broadcast_mac, 6) != 0);
}