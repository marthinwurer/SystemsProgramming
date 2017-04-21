#include <kern/net/intel.h>


uint32_t intel_csr_read32(uint32_t csr_addr, uint32_t offset) {
	return *((volatile uint32_t*) csr_addr + offset);
}

uint16_t intel_csr_read16(uint32_t csr_addr, uint32_t offset) {
	return *((volatile uint16_t*) csr_addr + offset);
}

uint8_t intel_csr_read8(uint32_t csr_addr, uint32_t offset) {
	return *((volatile uint8_t*) csr_addr + offset);
}


void intel_csr_write32(uint32_t csr_addr, uint32_t offset, uint32_t data) {
	*((volatile uint32_t*) csr_addr + offset) = data;
}

void intel_csr_write16(uint32_t csr_addr, uint32_t offset, uint16_t data) {
	*((volatile uint16_t*) csr_addr + offset) = data;
}

void intel_csr_write8(uint32_t csr_addr, uint32_t offset, uint8_t data) {
	*((volatile uint8_t*) csr_addr + offset) = data;
}


// TODO: Use PCI functions to find MMIO range and write functions to do stuff with this
