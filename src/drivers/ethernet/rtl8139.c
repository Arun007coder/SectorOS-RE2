#include "rtl8139.h"

int isRTL8139Init = 0;

pci_t pci_rtl8139_dev;
rtl8139_dev_t rtl8139_dev;

uint32_t current_packet_ptr;

uint8_t TSAD_array[4] = {0x20, 0x24, 0x28, 0x2C};
uint8_t TSD_array[4] = {0x10, 0x14, 0x18, 0x1C};

void receive_packet()
{
    uint16_t * t = (uint16_t*)(rtl8139_dev.rx_buf + current_packet_ptr);
    uint16_t packet_length = *(t + 1);

    t = t + 2;

    void * packet = kmalloc(packet_length);
    memcpy(packet, t, packet_length);
    interface_receive(packet, packet_length);

    current_packet_ptr = (current_packet_ptr + packet_length + 4 + 3) & RX_READ_POINTER_MASK;

    if(current_packet_ptr > RX_BUF_SIZE)
        current_packet_ptr -= RX_BUF_SIZE;

    outw(rtl8139_dev.io_base + CAPR, current_packet_ptr - 0x10);
}

void rtl8139_handler(registers_t * reg)
{
    uint16_t status = inw(rtl8139_dev.io_base + 0x3e);

    if(status & TOK)
    {
        printf("Packet sent\n");
    }

    if (status & ROK)
    {
        receive_packet();
    }


    outw(rtl8139_dev.io_base + 0x3E, 0x5);
}

void read_mac_addr()
{
    uint32_t mac_part1 = inl(rtl8139_dev.io_base + 0x00);
    uint16_t mac_part2 = inw(rtl8139_dev.io_base + 0x04);
    rtl8139_dev.mac_addr[0] = mac_part1 >> 0;
    rtl8139_dev.mac_addr[1] = mac_part1 >> 8;
    rtl8139_dev.mac_addr[2] = mac_part1 >> 16;
    rtl8139_dev.mac_addr[3] = mac_part1 >> 24;

    rtl8139_dev.mac_addr[4] = mac_part2 >> 0;
    rtl8139_dev.mac_addr[5] = mac_part2 >> 8;
    printf("MAC Address: %01x:%01x:%01x:%01x:%01x:%01x\n", rtl8139_dev.mac_addr[0], rtl8139_dev.mac_addr[1], rtl8139_dev.mac_addr[2], rtl8139_dev.mac_addr[3], rtl8139_dev.mac_addr[4], rtl8139_dev.mac_addr[5]);
}

void get_mac_addr(uint8_t * src_mac_addr) {
    memcpy(src_mac_addr, rtl8139_dev.mac_addr, 6);
}

void rtl8139_send(void * data, uint32_t len)
{
    void * transfer_data = kmalloc_a(len);
    void * phys_addr = virt2phys(kernel_page_dir, transfer_data);
    memcpy(transfer_data, data, len);

    outl(rtl8139_dev.io_base + TSAD_array[rtl8139_dev.tx_cur], (uint32_t)phys_addr);
    outl(rtl8139_dev.io_base + TSD_array[rtl8139_dev.tx_cur++], len);
    if(rtl8139_dev.tx_cur > 3)
        rtl8139_dev.tx_cur = 0;
}

void rtl8139_reset()
{
    outb(rtl8139_dev.io_base + 0x37, 0x10);
    while((inb(rtl8139_dev.io_base + 0x37) & 0x10) != 0)
    {
    }

    rtl8139_dev.rx_buf = kmalloc(8192 + 16 + 1500);
    memset(rtl8139_dev.rx_buf, 0x0, 8192 + 16 + 1500);
    outl(rtl8139_dev.io_base + 0x30, (uint32_t)virt2phys(kernel_page_dir,rtl8139_dev.rx_buf));

    outw(rtl8139_dev.io_base + 0x3C, 0x0005);

    outl(rtl8139_dev.io_base + 0x44, 0xf | (1 << 7));

    outb(rtl8139_dev.io_base + 0x37, 0x0C);
}

void init_rtl8139()
{
    if(pci_isDeviceAvailable(RTL8139_VENDOR_ID, RTL8139_DEVICE_ID) == 0)
    {
        printf("[RTL8139] Device not found\n");
        return;
    }
    if(isRTL8139Init == 0)
    {
        pci_rtl8139_dev = pci_get_device(RTL8139_VENDOR_ID, RTL8139_DEVICE_ID, -1);
        uint32_t ret = pci_read(pci_rtl8139_dev, PCI_OFF_BAR0);
        rtl8139_dev.bar_type = ret & 0x1;
        rtl8139_dev.io_base = ret & (~0x3);
        rtl8139_dev.mem_base = ret & (~0xf);
        printl("[RTL8139] use %s access (base: %x)\n", (rtl8139_dev.bar_type == 0)? "mem based":"port based", (rtl8139_dev.bar_type != 0)?rtl8139_dev.io_base:rtl8139_dev.mem_base);

        rtl8139_dev.tx_cur = 0;

        uint32_t pci_command_reg = pci_read(pci_rtl8139_dev, PCI_OFF_COMMAND);
        if(!(pci_command_reg & (1 << 2)))
        {
            pci_command_reg |= (1 << 2);
            pci_write(pci_rtl8139_dev, PCI_OFF_COMMAND, pci_command_reg);
        }

        outb(rtl8139_dev.io_base + 0x52, 0x0);

        outb(rtl8139_dev.io_base + 0x37, 0x10);
        while((inb(rtl8139_dev.io_base + 0x37) & 0x10) != 0)
        {
        }

        rtl8139_dev.rx_buf = kmalloc(8192 + 16 + 1500);
        memset(rtl8139_dev.rx_buf, 0x0, 8192 + 16 + 1500);
        outl(rtl8139_dev.io_base + 0x30, (uint32_t)virt2phys(kernel_page_dir,rtl8139_dev.rx_buf));

        outw(rtl8139_dev.io_base + 0x3C, 0x0005);

        outl(rtl8139_dev.io_base + 0x44, 0xf | (1 << 7));

        outb(rtl8139_dev.io_base + 0x37, 0x0C);

        uint32_t irq_num = pci_read(pci_rtl8139_dev, PCI_OFF_INTERRUPT_LINE);
        register_interrupt_handler(32 + irq_num, rtl8139_handler);
        printl("[RTL8139] Registered irq interrupt for rtl8139, irq num = %d\n", irq_num);

        read_mac_addr();

        net_t * net = kmalloc(sizeof(net_t));
        net->IUID = RTL8139_IUID;
        net->send = rtl8139_send;
        net->getmac = rtl8139_get_mac_addr;
        net->getip = rtl8139_get_ip_addr;
        net->setip = rtl8139_set_ip_addr;

        register_network_interface(net);

        printf("[RTL8139] Initialized...\n");

        isRTL8139Init = 1;
    }
    else
    {
        return;
    }
}

uint64_t rtl8139_get_mac_addr()
{
    return *(uint64_t *)rtl8139_dev.mac_addr;
}

uint64_t rtl8139_get_ip_addr()
{
    return *(uint64_t *)rtl8139_dev.ip_addr;
}

void rtl8139_set_ip_addr(uint64_t ip_addr)
{
    memcpy(rtl8139_dev.ip_addr, &ip_addr, 4);
}