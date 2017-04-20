#include <kern/pci/pci.h>

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

int16_t pci_get_slot(uint16_t vendor, uint16_t device) {
	// Check 32 slots then give up
	for (int slot = 0; slot < 32; slot++) {
		if(pci_cfg_read_word(0, slot, 0, PCI_VENDOR) == vendor) {
			if(pci_cfg_read_word(0, slot, 0, PCI_DEVICE) == device) {
				return slot;
			}
		}
	}
	return -1;
	// c_printf(" --- HIT! vendor: 0x%04x, device: 0x%04x\n", vendor, device);
	// printf("Checking PCI, bus=%d, slot=%d", 0, slot);
	// pci_get_vendor(0, slot);
}

void pci_enumerate(uint8_t bus, uint8_t max_entries) {
	// uint16_t vendor, device;
	uint32_t vendor, device;
	for (int slot = 0; slot < max_entries; slot++) {
		if ((vendor = pci_cfg_read_word(bus, slot, 0, PCI_VENDOR)) != 0xFFFF) {
			device = pci_cfg_read_word(bus, slot, 0, PCI_DEVICE);
			c_printf("PCI slot: %d, vendor: 0x%04x, device: 0x%04x\n", slot, vendor, device);
		}
	}
}
