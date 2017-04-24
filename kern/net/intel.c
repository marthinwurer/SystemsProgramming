#include <kern/net/intel.h>
#include <kern/pci/pci.h>

#include <baseline/c_io.h>

#define TRUE 1
#define FALSE 0

static uint8_t bus;
static uint8_t slot;
static uint32_t csr_base_addr;
static uint32_t eeprom_exists;
static uint8_t mac_addr[6];

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

void print_mac_addr(uint8_t mac[]) {
	c_printf("%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

uint32_t detect_eeprom()
{
    uint16_t val = 0;
    uint32_t exists = FALSE;
    intel_csr_write16(NET_INTEL_EEPROM_CTRL_REG, 0x1); 
    for(int i = 0; i < 1000 && !exists; i++) {
        val = intel_csr_read16(NET_INTEL_EEPROM_CTRL_REG);
        if(val & 0x10) exists = TRUE;
        else exists = FALSE;
    }
    return exists;
}
 
uint16_t eeprom_read(uint8_t addr)
{
	uint16_t tmp = 0;
        if(eeprom_exists) {
            	intel_csr_write16(NET_INTEL_EEPROM_CTRL_REG, (1) | ((uint32_t)(addr) << 8) );
        	while(!((tmp = intel_csr_read16(NET_INTEL_EEPROM_CTRL_REG)) & (1 << 4)));
        } 
        else {
            intel_csr_write16(NET_INTEL_EEPROM_CTRL_REG, (1) | ((uint32_t)(addr) << 2) );
            while(!((tmp = intel_csr_read16(NET_INTEL_EEPROM_CTRL_REG)) & (1 << 1)));
        }
	return (uint16_t) ((tmp >> 16) & 0xFFFF);
}

uint32_t read_mac_addr(uint8_t* mac[]) {
    if(eeprom_exists){
        uint16_t temp;
        temp = eeprom_read(0);
        (*mac)[0] = (uint8_t) (temp & 0xFF);
        (*mac)[1] = (uint8_t) (temp >> 8);
        temp = eeprom_read(1);
        (*mac)[2] = (uint8_t) (temp & 0xFF);
        (*mac)[3] = (uint8_t) (temp >> 8);
        temp = eeprom_read(2);
        (*mac)[4] = (uint8_t) (temp & 0xFF);
        (*mac)[5] = (uint8_t) (temp >> 8);
    }
    else {
        uint8_t * mem_base_mac_8 = (uint8_t*) (0x5400);
        uint32_t * mem_base_mac_32 = (uint32_t*) (0x5400);
        if (mem_base_mac_32[0] != 0 ) {
            for(int i = 0; i < 6; i++) {
                (*mac)[i] = mem_base_mac_8[i];
            }
        }
        else {
        	return FALSE; // failure
        } 
    }
    return TRUE; // success
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

	for(uint32_t i = csr_base_addr; i < csr_base_addr + 0x40; i += 8) {
		c_printf("addr: 0x%08x --> 0x%08x, ", i, *((uint32_t*)i));
		c_printf("addr: 0x%08x --> 0x%08x\n", (i + 4), *((uint32_t*) (i + 4)));
	}

	uint8_t command_byte = intel_csr_read8(NET_INTEL_SCB_COMMAND_BYTE);
	uint8_t interrupt_control_byte = intel_csr_read8(NET_INTEL_SCB_COMMAND_BYTE + 1);
	c_printf("command_byte: 0x%02x\n", command_byte);
	c_printf("interrupt_control_byte: 0x%02x\n", interrupt_control_byte);
	
	uint32_t cmd_byte_addr = csr_base_addr + NET_INTEL_SCB_COMMAND_BYTE;
	c_printf("command_byte&interrupt_byte(0x%08x): 0x%04x\n", cmd_byte_addr, *((uint16_t*) cmd_byte_addr));
	c_printf("command_byte(0x%08x): 0x%02x\n", cmd_byte_addr, *((uint8_t*) cmd_byte_addr));
	cmd_byte_addr = csr_base_addr + NET_INTEL_SCB_COMMAND_BYTE;
	c_printf("interrupt_byte(0x%08x): 0x%02x\n", cmd_byte_addr, (uint8_t) (*((uint16_t*) cmd_byte_addr) >> 8));

	//
	//
	//
	//
	//
	//
	// TODO: ensure intel_csr_* functions are operating as expected
	// currently, intel_csr_write8 and intel_csr_read8 don't really work,
	// could be an odd alignment thing, but it needs serious fixing
	//
	// (uint8_t) (*((uint16_t*) (csr_base_addr + offset) >> ((offset & 1) * 8))
	//
	//
	//
	//

	eeprom_exists = detect_eeprom();
	read_mac_addr((uint8_t**) &mac_addr); // suppress warnings with a cast
	print_mac_addr(mac_addr);


	uint32_t cbl_ptr = (uint32_t) dumb_malloc(12);
	// TODO: fill in command block with configure command
	//
	//
	// Write out pointer to CBL (Command Block List)
	intel_csr_write32(NET_INTEL_SCB_GENERAL_PTR, cbl_ptr);

	// Write out CU Start to begin executing the first CB in the CBL
	command_byte &= 0x07; // XXXXXXXX --> 00000XXX
	command_byte |= 0x10; // 00000XXX --> 00010XXX
	intel_csr_write8(NET_INTEL_SCB_COMMAND_BYTE, command_byte);

	// Make another command block to write out data or some other command and CU start again
}

uint32_t intel_csr_read32(uint8_t offset) {
	return *((volatile uint32_t*) (csr_base_addr + offset));
}

uint16_t intel_csr_read16(uint8_t offset) {
	return *((volatile uint16_t*) (csr_base_addr + offset));
}

uint8_t intel_csr_read8(uint8_t offset) {
	return *((volatile uint8_t*) (csr_base_addr + offset));
}


void intel_csr_write32(uint8_t offset, uint32_t data) {
	*((volatile uint32_t*) (csr_base_addr + offset)) = data;
}

void intel_csr_write16(uint8_t offset, uint16_t data) {
	*((volatile uint16_t*) (csr_base_addr + offset)) = data;
}

void intel_csr_write8(uint8_t offset, uint8_t data) {
	*((volatile uint8_t*) (csr_base_addr + offset)) = data;
}
