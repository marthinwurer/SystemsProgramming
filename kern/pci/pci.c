#include <kern/pci/pci.h>
#include <baseline/c_io.h>

uint32_t pci_cfg_read(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
	uint32_t address;
	uint32_t lbus  = (uint32_t)bus;
	uint32_t lslot = (uint32_t)slot;
	uint32_t lfunc = (uint32_t)func;
	uint32_t tmp = 0;

	address = (uint32_t)((lbus << 16) | (lslot << 11) |
			(lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));

	__outl(0xCF8, address);
	tmp = (uint32_t) __inl(0xCFC);
	// c_printf("pci_cfg_read(%d, %d, %d, %d) = 0x%08x\n", bus, slot, func, offset, tmp);
	return tmp;
}

uint16_t pci_cfg_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
	// (offset & 0x02) = {0, 2}, wihch is the offset into the current 32 bit long
	return (uint16_t) ((pci_cfg_read(bus, slot, func, offset) >> ((offset & 0x02) * 8)) & 0xFFFF);
}

uint8_t pci_cfg_read_byte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
	// (offset & 0x03) = {0, 1, 2, 3}, wihch is the offset into the current 32 bit long
	return (uint8_t) ((pci_cfg_read(bus, slot, func, offset) >> ((offset & 0x03) * 8)) & 0xFF);
}

uint16_t pci_get_vendor(uint8_t bus, uint8_t slot) {
	uint16_t vendor, device;
	/* try and read the first configuration register. Since there are no */
	/* vendors that == 0xFFFF, it must be a non-existent device. */
	if ((vendor = (uint16_t) pci_cfg_read(bus,slot,0,PCI_VENDOR)) != 0xFFFF) {
		device = (uint16_t) pci_cfg_read(bus,slot,0,PCI_DEVICE);
		c_printf(" --- HIT! vendor: 0x%04x, device: 0x%04x\n", vendor, device);
	}
	else {
		c_printf("\n");
	}
	return (vendor);
}

int32_t pci_get_device(uint8_t* bus_ret, uint8_t* slot_ret, uint16_t vendor, uint16_t device) {
	for (uint8_t bus = 0; bus < 8; bus++) {
		for (uint8_t slot = 0; slot < 32; slot++) {
			if(pci_cfg_read_word(bus, slot, 0, PCI_VENDOR) == vendor) {
				if(pci_cfg_read_word(bus, slot, 0, PCI_DEVICE) == device) {
					*bus_ret = bus;
					*slot_ret = slot;
					return 0;
				}
			}
		}
	}
	return -1;
}

void pci_enumerate(uint8_t max_bus, uint8_t max_slot) {
	// uint16_t vendor, device;
	uint32_t vendor, device;
	for (int bus = 0; bus < max_bus; bus++) {
		for (int slot = 0; slot < max_slot; slot++) {
			if ((vendor = pci_cfg_read_word(bus, slot, 0, PCI_VENDOR)) != 0xFFFF) {
				device = pci_cfg_read_word(bus, slot, 0, PCI_DEVICE);
				c_printf("PCI bus: %d, slot: %d, vendor: 0x%04x, device: 0x%04x\n", bus, slot, vendor, device);
			}
		}
	}
}
