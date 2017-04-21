#include <kern/net/net_test.h>
#include <kern/pci/pci.h>
#include <kern/net/intel.h>

#include <baseline/c_io.h>


void* next_malloc_addr = 0xC0000000; // start at 3GB, hope for the best
void* dumb_malloc(uint32_t size) {
	uint8_t* ptr = (uint8_t*) next_malloc_addr;
	for(int i = 0; i < size; i++) {
		if(*ptr != 0) {
			return NULL;
		}
		ptr++;
	}
	ptr = next_malloc_addr;
	next_malloc_addr += size;
	return (void*) ptr;
}

int32_t net_test_main(void* args) {
	(void) args; // surpress warnings
	c_printf("in net_test_main()\n");

	pci_enumerate(8, 16);

	uint8_t slot;
	uint8_t bus;
	uint16_t vendor = NET_INTEL_VENDOR; // Intel
	uint16_t device = NET_INTEL_QEMU_NIC; // QEMU net card
	// uint16_t device = NET_INTEL_DSL_NIC; // DSL net card

	if(pci_get_device(&bus, &slot, vendor, device) >= 0) {
		c_printf("Intel NIC -- bus: %d, slot: %d, vendor: 0x%04x, device: 0x%04x\n", bus, slot, vendor, device);

		uint32_t csr_base = pci_cfg_read(bus, slot, 0, PCI_CSR_MEM_MAPPED_BASE_ADDR_REG);
		c_printf("Intel NIC Control Status Register Base Addr = 0x%08x\n", csr_base);

		// will need to issue "configure" commands before using this device

		// figure out MAC address

		// set up Control block list (use bad malloc)

		// Write out CU command to SCB

	}
	return 0;
}

