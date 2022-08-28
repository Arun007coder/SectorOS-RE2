#include "dhcp.h"

char ip_addr[4];
int is_ip_allocated;

char hostname[64] = {0};
uint32_t hostname_size = 0;

int isStatic = 0;

int isIPRequested = 0;

int gethostaddr(char * addr)
{
    memcpy(addr, ip_addr, 4);
    if(!is_ip_allocated)
    {
        return 0;
    }
    return 1;
}

int isIPReady()
{
    if(!is_ip_allocated)
    {
        return 0;
    }
    return 1;
}

void setStaticIP(uint8_t* ip)
{
    isStatic = 1;
    dhcp_discover();
    dhcp_request(ip);
}

void setHostName(char* Hostname)
{
    uint32_t size = strlen(Hostname);
    memcpy(hostname, Hostname, size);
    hostname_size = size;
    printf("[DHCP] Hostname changed to %s!\n", hostname);
}

void dhcp_discover()
{
    uint8_t request_ip[4];
    uint8_t dst_ip[4];
    memset(request_ip, 0x0, 4);
    memset(dst_ip, 0xff, 4);
    dhcp_packet_t * packet = kmalloc(sizeof(dhcp_packet_t));
    memset(packet, 0, sizeof(dhcp_packet_t));
    printf("[DHCP] DHCPDISCOVER to 255.255.255.255 port 67\n");
    make_dhcp_packet(packet, 1, request_ip);
    udp_send_packet(dst_ip, 68, 67, packet, sizeof(dhcp_packet_t));
}

void dhcp_request(uint8_t * request_ip)
{
    uint8_t dst_ip[4];
    memset(dst_ip, 0xff, 4);
    dhcp_packet_t * packet = kmalloc(sizeof(dhcp_packet_t));
    memset(packet, 0, sizeof(dhcp_packet_t));
    printf("[DHCP] DHCPREQUEST on %d.%d.%d.%d to %d.%d.%d.%d port 67\n", request_ip[0], request_ip[1], request_ip[2], request_ip[3], dst_ip[0], dst_ip[1], dst_ip[2], dst_ip[3]);
    make_dhcp_packet(packet, 3, request_ip);
    udp_send_packet(dst_ip, 68, 67, packet, sizeof(dhcp_packet_t));
}

void dhcp_udp_callback(ipv4_addr_t* srcaddr, uint8_t* data, uint32_t len)
{
    dhcp_packet_t * packet = (dhcp_packet_t*)data;
    dhcp_handle_packet(packet);
}

void dhcp_handle_packet(dhcp_packet_t * packet)
{
    uint8_t * options = packet->options + 4;
    uint8_t* requested_ip = kcalloc(sizeof(uint8_t), 4);
    printf("[DHCP] Packet got!\n");
    if(packet->op == DHCP_REPLY)
    {
        uint8_t * type = get_dhcp_options(packet, 53);
        if(*type == 2)
        {
            if(isStatic != 1)
            {
                iptoa(packet->your_ip, requested_ip);
                dhcp_request(((uint8_t*)&packet->your_ip));

            }
        }
        else if(*type == 6)
        {
            printf("[DHCP] (NAK) Request of %d.%d.%d.%d rejected. Trying to get new ip!\n", requested_ip[0], requested_ip[1], requested_ip[2], requested_ip[3]);
            memset(requested_ip, 0, 4);
            if(isStatic == 1)
            {
                isStatic = 0;
            }
            dhcp_discover();
        }
        else if (*type == 5)
        {
            memcpy(ip_addr, &packet->your_ip, 4);
            printf("[DHCP] (ACK) Request for %d.%d.%d.%d accepted. Setting IP!\n", ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]);
            set_ip(ip_addr);
            is_ip_allocated = 1;
        }
    }
}

void * get_dhcp_options(dhcp_packet_t * packet, uint8_t type)
{
    uint8_t * options = packet->options + 4;
    uint8_t curr_type = *options;
    while(curr_type != 0xff)
    {
        uint8_t len = *(options + 1);
        if(curr_type == type)
        {
            void * ret = kmalloc(len);
            memcpy(ret, options + 2, len);
            return ret;
        }
        options += (2 + len);
    }
}

void make_dhcp_packet(dhcp_packet_t * packet, uint8_t msg_type, uint8_t * request_ip)
{
    packet->op = DHCP_REQUEST;
    packet->hardware_type = HARDWARE_TYPE_ETHERNET;
    packet->hardware_addr_len = 6;
    packet->hops = 0;
    packet->xid = htonl(DHCP_TRANSACTION_ID);
    packet->flags = htons(0x8000);
    get_mac(packet->client_hardware_addr);

    uint8_t dst_ip[4];
    memset(dst_ip, 0xff, 4);

    uint8_t * options = packet->options;
    *((uint32_t*)(options)) = htonl(0x63825363);
    options += 4;

    *(options++) = 53;
    *(options++) = 1;
    *(options++) = msg_type;

    *(options++) = 61;
    *(options++) = 0x07;
    *(options++) = 0x01;
    get_mac(options);
    options += 6;

    *(options++) = 50;
    *(options++) = 0x04;
    *((uint32_t*)(options)) = htonl(0x0a00020e);
    memcpy((uint32_t*)(options), request_ip, 4);
    options += 4;

    *(options++) = 12;
    *(options++) = hostname_size;
    memcpy(options, hostname, hostname_size);
    options += hostname_size;
    *(options++) = 0x00;

    *(options++) = 55;
    *(options++) = 8;
    *(options++) = 0x1;
    *(options++) = 0x3;
    *(options++) = 0x6;
    *(options++) = 0xf;
    *(options++) = 0x2c;
    *(options++) = 0x2e;
    *(options++) = 0x2f;
    *(options++) = 0x39;
    *(options++) = 0xff;
}

void init_dhcp()
{
    is_ip_allocated = 0;
    memcpy("SectorOS_RE2", hostname, 13);
    hostname_size = 13;
}