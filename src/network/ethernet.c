#include "ethernet.h"
#include "xxd.h"

int ethernet_send(uint8_t* dst_macaddr, void* data, int size, uint16_t protocol)
{
    uint8_t src_macaddr[6];

    void *packet = kmalloc(sizeof(etherframe_t) + size);
    memset(packet, 0, sizeof(etherframe_t) + size);
    void *Pdata = packet + sizeof(etherframe_t);

    etherframe_t* frame = (etherframe_t*)packet;

    get_mac(src_macaddr);

    memcpy(frame->dst_macaddr, dst_macaddr, 6);
    memcpy(frame->src_macaddr, src_macaddr, 6);

    memcpy(Pdata, data, size);

    frame->type = htons(protocol);

    interface_send(packet, (sizeof(etherframe_t) + size));
    kfree(packet);

    return size;
}

void ethernet_handle_packet(etherframe_t* packet, int size)
{
    void * data = (void*) packet + sizeof(etherframe_t);
    int data_len = size - sizeof(etherframe_t);
    if(ntohs(packet->type) == ETHERNET_TYPE_ARP)
    {
        printf("(ARP Packet)\n");
        arp_handle_packet(data, data_len);
    }
    if(ntohs(packet->type) == ETHERNET_TYPE_IP)
    {
        printf("(IP Packet)\n");
        ip_handle_packet(data);
    }
}