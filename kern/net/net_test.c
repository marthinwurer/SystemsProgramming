#include <kern/net/net_test.h>
#include <kern/pci/pci.h>

int32_t net_test_main(void* args) {
	c_printf("in net_test_main()\n");

	pci_enumerate(0, 16);

	// QEMU ONLY!!!
	int16_t slot_e;
	uint8_t slot;	 
	uint16_t vendor = 0x8086; // Intel
	uint16_t device = 0x100E; // QEMU net card
	// uint8_t device = 0x1229; // DSL net card
	// c_printf("", pci_get_slot(vendor))
	if((slot_e = pci_get_slot(vendor, device)) > 0) {
		slot = (uint8_t) slot_e;

		uint32_t mem_map_base_addr = pci_cfg_read(0, slot, 0, PCI_CSR_MEM_MAPPED_BASE_ADDR_REG);
		uint32_t io = pci_cfg_read(0, slot, 0, PCI_CSR_IO_MAPPED_BASE_ADDR_REG);
		uint32_t flash = pci_cfg_read(0, slot, 0, PCI_FLASH_MEM_MAPPED_BASE_ADDR_REG);

		c_printf("Intel 100E Network Card found in slot: %d\n", slot);
		c_printf("PCI_CSR_MEM_MAPPED_BASE_ADDR_REG = 0x%08x\n", mem);
		c_printf("PCI_CSR_IO_MAPPED_BASE_ADDR_REG = 0x%08x\n", io);
		c_printf("PCI_FLASH_MEM_MAPPED_BASE_ADDR_REG = 0x%08x\n", flash);
		// TODO: mess with the pci config stuff now that we know what slot the network card is in
	}
	return 0;
}

