#include <kern/net/intel.h>
#include <kern/pci/pci.h>

#include <baseline/c_io.h>

static uint8_t bus;
static uint8_t slot;
static uint32_t csr_base_addr;

uint32_t base_free = 0x3A00; // this seems safe...
uint32_t free_top = 0x7BFF;
void* dumb_malloc(uint32_t size) {
	if(base_free + size > free_top) {
		return NULL;
	}
	void* ret = (void*) base_free;
	base_free += size;
	return ret;
}

void intel_nic_init() {
	// Try to find DSL card first, followed by QEMU card, then give up
	if(pci_get_device(&bus, &slot, NET_INTEL_VENDOR, NET_INTEL_DSL_NIC) >= 0) {	}
	else if(pci_get_device(&bus, &slot, NET_INTEL_VENDOR, NET_INTEL_QEMU_NIC) < 0) {
		c_printf("Could not find Intel NIC\n");
		return;
	}

	c_printf("Intel NIC found - bus: %d, slot: %d\n", bus, slot);

	csr_base_addr = pci_cfg_read(bus, slot, 0, PCI_CSR_MEM_MAPPED_BASE_ADDR_REG);
	// c_printf("Intel NIC Control Status Register Base Addr = 0x%08x\n", csr_base_addr);

	// for(uint32_t i = csr_base_addr; i < csr_base_addr + 0x60; i += 8) {
	// 	c_printf("addr: 0x%08x --> 0x%08x, ", i, *((uint32_t*)i));
	// 	c_printf("addr: 0x%08x --> 0x%08x\n", (i + 4), *((uint32_t*) (i + 4)));
	// }

	uint8_t command_byte = intel_csr_read8(NET_INTEL_SCB_COMMAND_BYTE);
	uint8_t interrupt_control_byte = intel_csr_read8(NET_INTEL_SCB_COMMAND_BYTE + 1);
	c_printf("command_byte: 0x%02x\n", command_byte);
	c_printf("interrupt_control_byte: 0x%02x\n", interrupt_control_byte);

	// will need to issue "configure" commands before using this device

	// figure out MAC address

	// set up Control block list (use bad malloc)

	// Write out CU command to SCB

}

uint32_t intel_csr_read32(uint8_t offset) {
	return *((volatile uint32_t*) csr_base_addr + offset);
}

uint16_t intel_csr_read16(uint8_t offset) {
	return *((volatile uint16_t*) csr_base_addr + offset);
}

uint8_t intel_csr_read8(uint8_t offset) {
	return *((volatile uint8_t*) csr_base_addr + offset);
}


void intel_csr_write32(uint8_t offset, uint32_t data) {
	*((volatile uint32_t*) csr_base_addr + offset) = data;
}

void intel_csr_write16(uint8_t offset, uint16_t data) {
	*((volatile uint16_t*) csr_base_addr + offset) = data;
}

void intel_csr_write8(uint8_t offset, uint8_t data) {
	*((volatile uint8_t*) csr_base_addr + offset) = data;
}
