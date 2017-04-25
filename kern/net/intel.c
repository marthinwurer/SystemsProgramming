#include <kern/net/intel.h>
#include <kern/pci/pci.h>

#include <baseline/ulib.h>
#include <baseline/c_io.h>

#define TRUE 1
#define FALSE 0



// Holds info about the network interface
static struct nic_info _nic;

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

static void write_flush(struct nic_info *nic) {
	(void) mem_read8(&nic->csr->scb.status);
}

static uint16_t eeprom_read(struct nic_info *nic, uint16_t *addr_len, uint16_t addr)
{
	uint32_t cmd_addr_data;
	uint16_t data = 0;
	uint8_t ctrl;
	int32_t i;

	cmd_addr_data = ((op_read << *addr_len) | addr) << 16;

	/* Chip select */
	mem_write8(&nic->csr->eeprom_lo, EECS | EESK);
	write_flush(nic); sleep(1);

	/* Bit-bang to read word from eeprom */
	for (i = 31; i >= 0; i--) {
		ctrl = (cmd_addr_data & (1 << i)) ? EECS | EEDI : EECS;
		mem_write8(&nic->csr->eeprom_lo, ctrl);
		write_flush(nic); sleep(1);

		mem_write8(&nic->csr->eeprom_lo, ctrl | EESK);
		write_flush(nic); sleep(1);

		/* Eeprom drives a dummy zero to EEDO after receiving
		 * complete address.  Use this to adjust addr_len. */
		ctrl = mem_read8(&nic->csr->eeprom_lo);
		if (!(ctrl & EEDO) && i > 16) {
			*addr_len -= (i - 16);
			i = 17;
		}

		data = (data << 1) | (ctrl & EEDO ? 1 : 0);
	}

	/* Chip deselect */
	mem_write8(&nic->csr->eeprom_lo, 0);
	write_flush(nic); sleep(1);

	return data;
}

/* Load entire EEPROM image into driver cache and validate checksum */
static void eeprom_load(struct nic_info *nic)
{
	uint16_t addr, addr_len = 8, checksum = 0;

	/* Try reading with an 8-bit addr len to discover actual addr len */
	eeprom_read(nic, &addr_len, 0);
	nic->eeprom_count = 1 << addr_len;

	for (addr = 0; addr < nic->eeprom_count; addr++) {
		nic->eeprom[addr] = eeprom_read(nic, &addr_len, addr);
		if (addr < nic->eeprom_count - 1) {
			// checksum += le16_to_cpu(nic->eeprom[addr]);
			checksum += nic->eeprom[addr];
		}
	}

	/* The checksum, stored in the last word, is calculated such that
	 * the sum of words should be 0xBABA */
	if ((0xBABA - checksum) != nic->eeprom[nic->eeprom_count - 1]) {
		c_printf("~~~ EEPROM corrupt ~~~\n");
	}
}

void intel_nic_init() {
	uint8_t bus, slot;
	// Try to find DSL card first, followed by QEMU card, then give up
	if(pci_get_device(&bus, &slot, NET_INTEL_VENDOR, NET_INTEL_DSL_NIC) >= 0) {	
		c_printf("QEMU Intel NIC found - bus: %d, slot: %d\n", bus, slot);
	}
	else if(pci_get_device(&bus, &slot, NET_INTEL_VENDOR, NET_INTEL_QEMU_NIC) >= 0) {
		c_printf("Lab Intel NIC found - bus: %d, slot: %d\n", bus, slot);
	}
	else {
		c_printf("Could not find Intel NIC\n");
		return;
	}

	_nic.csr = (struct csr *) pci_cfg_read(bus, slot, 0, PCI_CSR_MEM_MAPPED_BASE_ADDR_REG);
	c_printf("Intel NIC Control Status Register Base Addr = 0x%08x\n", (uint32_t) _nic.csr);

	// c_printf("status: 0x%02x\n", _nic.csr->scb.status);
	// c_printf("stat_ack: 0x%02x\n", _nic.csr->scb.stat_ack);
	// c_printf("command: 0x%02x\n", _nic.csr->scb.command);
	// c_printf("interrupt_mask: 0x%02x\n", _nic.csr->scb.interrupt_mask);
	// c_printf("gen_ptr: 0x%08x\n", _nic.csr->scb.gen_ptr);

	// load eeprom and print out what we get
	eeprom_load(&_nic);
	for(uint32_t i = 0; i < _nic.eeprom_count; i++) {
		c_printf("eeprom[%d] = 0x%04x\n", i, _nic.eeprom[i]);
	}

	// set base CU to 0
	mem_write32(&_nic.csr->scb.gen_ptr, 0);
	mem_write8(&_nic.csr->scb.command, cuc_load_cu_base);
	write_flush(&_nic);


	// page 83 of manual
	struct cb cbl;
	struct cb* cbl_ptr = &cbl;
	cbl_ptr->command = cb_el | cb_sf | cb_tx; // end of cbl, simplified mode, transmit
	// cbl_ptr->link = 0; Since the EL flag is set, we don't need to specfiy next ptr

	cbl_ptr->tcb.tbd_array = 0xFFFFFFFF; // for simplified mode, tbd is 1's, data is in tcb
	cbl_ptr->tcb.tcb_byte_count = 12 | 0x8000; // 0x8000 = EOF flag
	cbl_ptr->tcb.threshold = 1; // transmit once you have (1 * 8 bytes) in the queue
	cbl_ptr->tcb.tbd_count = 0;
	cbl_ptr->data1 = 0xDEADBEEF;
	cbl_ptr->data2 = 0x42424242;
	cbl_ptr->data3 = 0xBAAAAAAD;

	// give the cbl addr to the CU and start
	mem_write32(&_nic.csr->scb.gen_ptr, (uint32_t) cbl_ptr);
	mem_write8(&_nic.csr->scb.command, cuc_start);
	write_flush(&_nic);

	// 
	// 
	// TODO:
	// o Test transmitting data
	// o Write cleaner function to write data
	// o Get interrupts working
	// o configure receive buffers
	// o enable receiving data
	// o Write routine to output configure command blocks
	// 
	// 

}

uint32_t mem_read32(void* addr) {
	return *(volatile uint32_t*) addr;
}

uint16_t mem_read16(void* addr) {
	return *(volatile uint16_t*) addr;
}

uint8_t mem_read8(void* addr) {
	return *(volatile uint8_t*) addr;
}


void mem_write32(void* addr, uint32_t value) {
	*(volatile uint32_t*) addr = value;
}

void mem_write16(void* addr, uint16_t value) {
	*(volatile uint16_t*) addr = value;
}

void mem_write8(void* addr, uint8_t value) {
	*(volatile uint8_t*) addr = value;
}
