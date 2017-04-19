#include <kern/pci/pci.h>

uint16_t pci_cfg_read(uint8_t bus, uint8_t slot,
							uint8_t func, uint8_t offset) {
	uint32_t address;
	uint32_t lbus  = (uint32_t)bus;
	uint32_t lslot = (uint32_t)slot;
	uint32_t lfunc = (uint32_t)func;
	uint16_t tmp = 0;

	/* create configuration address as per Figure 1 */
	address = (uint32_t)((lbus << 16) | (lslot << 11) |
			(lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));

	/* write out the address */
	__outl(0xCF8, address);
	/* read in the data */
	/* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
	tmp = (uint16_t)((__inl(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
	return (tmp);
}

uint16_t pci_get_vendor(uint8_t bus, uint8_t slot) {
	uint16_t vendor, device;
	/* try and read the first configuration register. Since there are no */
	/* vendors that == 0xFFFF, it must be a non-existent device. */
	if ((vendor = pci_cfg_read(bus,slot,0,0)) != 0xFFFF) {
		device = pci_cfg_read(bus,slot,0,2);
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
		if (pci_cfg_read(bus,slot,0,0) != vendor) {
			if(pci_cfg_read(bus,slot,0,2) == device) {
				return slot;
			}
		}
	}
	return -1;
	// c_printf(" --- HIT! vendor: 0x%04x, device: 0x%04x\n", vendor, device);
	// printf("Checking PCI, bus=%d, slot=%d", 0, slot);
	// pci_get_vendor(0, slot);
}

void pci_enumerate(uint8_t bus) {
	uint16_t vendor, device;
	for (int slot = 0; slot < 16; slot++) {
		if ((vendor = pci_cfg_read(bus, slot, 0, PCI_VENDOR)) != 0xFFFF) {
			device = pci_cfg_read(bus, slot, 0, PCI_DEVICE);
			c_printf("PCI slot: %d, vendor: 0x%04x, device: 0x%04x\n", slot, vendor, device);
		}
	}
}
