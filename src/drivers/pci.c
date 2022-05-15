#include "pci.h"

uint32_t pci_size_map[100];
pci_t dev_zero = {0};

uint32_t pci_read(pci_t dev, uint32_t field)
{
    dev.field_num = (field & 0xFC) >> 2;
	dev.enable = 1;
	outl(PCI_CONFIG_PORT, dev.bits);

	uint32_t size = pci_size_map[field];
	if(size == 1)
    {
		uint8_t t = inb(PCI_DATA_PORT + (field & 3));
		return t;
	}
	else if(size == 2)
    {
		uint16_t t = inw(PCI_DATA_PORT + (field & 2));
		return t;
	}
	else if(size == 4)
    {
		uint32_t t = inl(PCI_DATA_PORT);
		return t;
	}
	return 0xffff;
}

void pci_write(pci_t dev, uint32_t field, uint32_t value)
{
    dev.field_num = (field & 0xFC) >> 2;
	dev.enable = 1;
	outl(PCI_CONFIG_PORT, dev.bits);
	outl(PCI_DATA_PORT, value);
}

uint32_t get_device_type(pci_t dev)
{
    uint32_t t = pci_read(dev, PCI_OFF_CLASS) << 8;
	return t | pci_read(dev, PCI_OFF_SUBCLASS);
}

uint32_t get_secondary_bus(pci_t dev)
{
    return pci_read(dev, PCI_OFF_SECONDARY_BUS);
}

uint32_t pci_reach_end(pci_t dev)
{
    uint32_t t = pci_read(dev, PCI_OFF_HEADER_TYPE);
	return !t;
}

pci_t pci_scan_function(uint16_t vendor_id, uint16_t device_id, uint32_t bus, uint32_t device, uint32_t function, int device_type)
{
    pci_t dev = {0};
	dev.bus_num = bus;
	dev.device_num = device;
	dev.function_num = function;

	if(get_device_type(dev) == PCI_TYPE_BRIDGE)
    {
		pci_scan_bus(vendor_id, device_id, get_secondary_bus(dev), device_type);
	}

	if(device_type == -1 || device_type == get_device_type(dev))
    {
		uint32_t devid  = pci_read(dev, PCI_OFF_DEVICE_ID);
		uint32_t vendid = pci_read(dev, PCI_OFF_VENDOR_ID);
		if(devid == device_id && vendor_id == vendid)
			return dev;
	}
	return dev_zero;
}

pci_t pci_scan_device(uint16_t vendor_id, uint16_t device_id, uint32_t bus, uint32_t device, int device_type)
{
    pci_t dev = {0};
	dev.bus_num = bus;
	dev.device_num = device;

	if(pci_read(dev,PCI_OFF_VENDOR_ID) == PCI_NONE)
		return dev_zero;

	pci_t t = pci_scan_function(vendor_id, device_id, bus, device, 0, device_type);
	if(t.bits)
		return t;

	if(pci_reach_end(dev))
		return dev_zero;

	for(int function = 1; function < FUNC_PER_DEV; function++)
    {
		if(pci_read(dev,PCI_OFF_VENDOR_ID) != PCI_NONE)
        {
			t = pci_scan_function(vendor_id, device_id, bus, device, function, device_type);
			if(t.bits)
				return t;
		}
	}
	return dev_zero;
}

pci_t pci_scan_bus(uint16_t vendor_id, uint16_t device_id, uint32_t bus, int device_type)
{
    for(int device = 0; device < DEV_PER_BUS; device++)
    {
		pci_t t = pci_scan_device(vendor_id, device_id, bus, device, device_type);
		if(t.bits)
			return t;
	}
	return dev_zero;
}

pci_t pci_get_device(uint16_t vendor_id, uint16_t device_id, int device_type)
{
    pci_t t = pci_scan_bus(vendor_id, device_id, 0, device_type);
	if(t.bits)
		return t;

	if(pci_reach_end(dev_zero))
    {
		printf("PCI device not found\n");
	}
	for(int function = 1; function < FUNC_PER_DEV; function++)
    {
		pci_t dev = {0};
		dev.function_num = function;

		if(pci_read(dev, PCI_OFF_VENDOR_ID) == PCI_NONE)
			break;
		t = pci_scan_bus(vendor_id, device_id, function, device_type);
		if(t.bits)
			return t;
	}
	return dev_zero;
}

void init_pci()
{
    pci_size_map[PCI_OFF_VENDOR_ID] = 2;
	pci_size_map[PCI_OFF_DEVICE_ID] = 2;
	pci_size_map[PCI_OFF_COMMAND] = 2;
	pci_size_map[PCI_OFF_STATUS] = 2;
	pci_size_map[PCI_OFF_SUBCLASS] = 1;
	pci_size_map[PCI_OFF_CLASS] = 1;
	pci_size_map[PCI_OFF_CACHE_LINE_SIZE] = 1;
	pci_size_map[PCI_OFF_LATENCY_TIMER]	= 1;
	pci_size_map[PCI_OFF_HEADER_TYPE] = 1;
	pci_size_map[PCI_OFF_BIST] = 1;
	pci_size_map[PCI_OFF_BAR0] = 4;
	pci_size_map[PCI_OFF_BAR1] = 4;
	pci_size_map[PCI_OFF_BAR2] = 4;
	pci_size_map[PCI_OFF_BAR3] = 4;
	pci_size_map[PCI_OFF_BAR4] = 4;
	pci_size_map[PCI_OFF_BAR5] = 4;
	pci_size_map[PCI_OFF_INTERRUPT_LINE] = 1;
	pci_size_map[PCI_OFF_SECONDARY_BUS]	= 1;
}