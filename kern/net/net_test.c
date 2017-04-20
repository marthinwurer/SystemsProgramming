#include <kern/net/net_test.h>
#include <kern/pci/pci.h>
#include <baseline/c_io.h>

int32_t net_test_main(void* args) {
	(void) args; // surpress warnings
	c_printf("in net_test_main()\n");

	pci_enumerate(8, 16);

	uint8_t slot;
	uint8_t bus;
	uint16_t vendor = 0x8086; // Intel
	// uint16_t device = 0x100E; // QEMU net card
	uint16_t device = 0x1229; // DSL net card

	if(pci_get_device(&bus, &slot, vendor, device) >= 0) {
		c_printf("Intel NIC -- bus: %d, slot: %d, vendor: 0x%04x, device: 0x%04x\n", bus, slot, vendor, device);

		uint32_t mem_map_base_addr = pci_cfg_read(bus, slot, 0, PCI_CSR_MEM_MAPPED_BASE_ADDR_REG);
		uint32_t io = pci_cfg_read(bus, slot, 0, PCI_CSR_IO_MAPPED_BASE_ADDR_REG);
		uint32_t flash = pci_cfg_read(bus, slot, 0, PCI_FLASH_MEM_MAPPED_BASE_ADDR_REG);

		c_printf("PCI_CSR_MEM_MAPPED_BASE_ADDR_REG = 0x%08x\n", mem_map_base_addr);
		c_printf("PCI_CSR_IO_MAPPED_BASE_ADDR_REG = 0x%08x\n", io);
		c_printf("PCI_FLASH_MEM_MAPPED_BASE_ADDR_REG = 0x%08x\n", flash);

		// decide between port and memory based accesses (probably MMIO)

		// mess with mem_map_base_addr CSR (Control Status Register)
		// will need to issue "configure" commands before using this device

	}
	return 0;
}

