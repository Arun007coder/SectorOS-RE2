#include "udp.h"
#include "dhcp.h"
#include "xxd.h"

udp_socket_t sockets[MAX_SOCKETS];
int usedSockets = 0;

udp_socket_t *get_socket_by_port(uint16_t port)
{
    for (int i = 0; i < usedSockets; i++)
    {
        if (sockets[i].port == port)
        {
            return &sockets[i];
        }
    }
    return NULL;
}

void register_udp_callback(udp_callback_t callback, uint16_t port)
{
    udp_socket_t *socket = get_socket_by_port(port);
    if (socket == NULL)
    {
        for (int i = 0; i < MAX_SOCKETS; i++)
        {
            if (sockets[i].port == 0)
            {
                sockets[i].port = port;
                sockets[i].callback = callback;
                usedSockets++;
                return;
            }
        }
        printE("[UDP] UDP callback register full! %d/%d\n", usedSockets, MAX_SOCKETS);
    }
    else
    {
        socket->callback = callback;
    }
}

void udp_send_packet(uint8_t * dst_ip, uint16_t src_port, uint16_t dst_port, void * data, int len)
{
    int length = sizeof(udp_packet_t) + len;
    udp_packet_t * packet = kmalloc(length);
    memset(packet, 0, sizeof(udp_packet_t));
    packet->src_port = htons(src_port);
    packet->dst_port = htons(dst_port);
    packet->length = htons(length);
    packet->checksum = 0;

    memcpy((void*)packet + sizeof(udp_packet_t), data, len);
    ip_send_packet(dst_ip, packet, length, PROTOCOL_UDP);
}

void udp_handle_packet(udp_packet_t * packet, ipv4_addr_t * src_addr)
{
    uint16_t dst_port = ntohs(packet->dst_port);
    uint16_t src_port = ntohs(packet->src_port);
    uint16_t length = ntohs(packet->length);

    void * data_ptr = (void*)packet + sizeof(udp_packet_t);
    uint32_t data_len = length;

    if(dst_port == 68)
    {
        xxd(data_ptr, data_len);
        printf("(dhcp packet)\n");
        dhcp_handle_packet((dhcp_packet_t*)data_ptr);
        return;
    }

    udp_socket_t * socket = get_socket_by_port(dst_port);

    if (socket == NULL)
    {
        return;
    }
    else
    {
        socket->callback(src_addr, data_ptr, data_len);
    }
}

void init_udp()
{
    for (int i = 0; i < MAX_SOCKETS; i++)
    {
        sockets[i].port = 0;
    }
}