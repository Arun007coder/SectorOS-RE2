#include "icmp.h"

int isReplied = 0;

void request_echo_reply(uint8_t* ip)
{
    icmp_packet_t* packet = (icmp_packet_t*)kmalloc(64);
    memset(packet, 0, 64);
    packet->type = ICMP_TYPE_ECHO_REQUEST;
    packet->code = 0;
    packet->checksum = 0;
    packet->id = 8913;
    packet->seq = 5454;
    packet->checksum = checksum((uint16_t*)packet, 64);
    ip_send_packet(ip, packet, 64, PROTOCOL_ICMP);
}

void icmp_echo_reply(icmp_packet_t* packet, uint8_t* src_ip, uint16_t echo_id, uint16_t echo_seq)
{
    icmp_packet_t* reply = (icmp_packet_t*)kmalloc(64);
    memset(packet, 0, 64);
    printf("[ICMP] Echo reply to %d.%d.%d.%d\n", src_ip[0], src_ip[1], src_ip[2], src_ip[3]);
    reply->type = ICMP_TYPE_ECHO_REPLY;
    reply->code = 0;
    reply->checksum = 0;
    reply->id = echo_id;
    reply->seq = echo_seq;
    reply->checksum = checksum((uint16_t*)reply, 64);
    ip_send_packet(src_ip, reply, 64, PROTOCOL_ICMP);
}

void icmp_handle_packet(icmp_packet_t* packet, uint8_t* srcip)
{
    switch(packet->type)
    {
        case ICMP_TYPE_ECHO_REQUEST:
            if(isReplied == 0)
            {
                printf("[ICMP] Echo request from %d.%d.%d.%d\n", srcip[0], srcip[1], srcip[2], srcip[3]);
                icmp_echo_reply(packet, srcip, packet->id, packet->seq);
                isReplied = 100;
            }
            else
            {
                isReplied--;
            }
            break;
        case ICMP_TYPE_ECHO_REPLY:
            printf("[ICMP] Echo reply from %d.%d.%d.%d\n", srcip[0], srcip[1], srcip[2], srcip[3]);
            break;
        case ICMP_TYPE_DEST_UNREACHABLE:
            printf("[ICMP] Destination unreachable from %d.%d.%d.%d\n", srcip[0], srcip[1], srcip[2], srcip[3]);
            break;
        case ICMP_TYPE_TIME_EXCEEDED:
            printf("[ICMP] Time exceeded from %d.%d.%d.%d\n", srcip[0], srcip[1], srcip[2], srcip[3]);
            break;
        case ICMP_TYPE_BAD_PARAM:
            printf("[ICMP] Parameter problem from %d.%d.%d.%d\n", srcip[0], srcip[1], srcip[2], srcip[3]);
            break;
        case ICMP_TYPE_ROUTER_ADVERTISEMENT:
            printf("[ICMP] Router advertisement from %d.%d.%d.%d\n", srcip[0], srcip[1], srcip[2], srcip[3]);
            break;
        default:
            printf("[ICMP] Packet type %d\n", packet->type);
            break;
    }
}