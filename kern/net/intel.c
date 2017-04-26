#include <kern/net/intel.h>
#include <kern/pci/pci.h>
// #include <kern/memory/memory_map.h> //get_next_page, free_page

#include <baseline/ulib.h> //sleep
#include <baseline/c_io.h> //c_printf
#include <string.h> //memcpy
#include <x86arch.h> //PIC_EOI, PIC_SLAVE_CMD_PORT
#include <baseline/support.h> //__install_isr
#include <baseline/startup.h> //__outb


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

void dump_eeprom(struct nic_info* nic) {
	for(uint32_t i = 0; i < nic->eeprom_count; i++) {
		if(i % 6 == 0)
			c_printf("\neep:");
		c_printf(" [%d]=0x%04x", i, nic->eeprom[i]);
	}
}

static void write_flush(struct nic_info *nic) {
	(void) mem_read8(&nic->csr->scb.status);
}

static uint16_t eeprom_read(struct nic_info *nic, uint16_t *addr_len, uint16_t addr) {
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
static void eeprom_load(struct nic_info *nic) {
	uint16_t addr, addr_len = 8, checksum = 0;

	/* Try reading with an 8-bit addr len to discover actual addr len */
	eeprom_read(nic, &addr_len, 0);
	nic->eeprom_count = 1 << addr_len;

	for(addr = 0; addr < nic->eeprom_count; addr++) {
		nic->eeprom[addr] = eeprom_read(nic, &addr_len, addr);
		if(addr < nic->eeprom_count - 1) {
			checksum += nic->eeprom[addr];
		}
	}

	/* The checksum, stored in the last word, is calculated such that
	 * the sum of words should be 0xBABA */
	if((0xBABA - checksum) != nic->eeprom[nic->eeprom_count - 1]) {
		c_printf("~~~ EEPROM corrupt ~~~\n");
	}
}

void intel_nic_handler(int vector, int code) {
	(void) vector; (void) code;
	c_printf("INTERRUPT (v=%02x, c=%02x) -- Intel NIC, ");
	uint8_t stat_ack = mem_read8(&_nic.csr->scb.stat_ack);
	c_printf("stat_ack: 0x%02x\n", stat_ack);

	// mem_write8(&_nic.csr->scb.stat_ack, 0x00);
	// Check status bits and acknowledge them

	__outb(PIC_MASTER_CMD_PORT, PIC_EOI); // slave because we are above 0x20
	__outb(PIC_SLAVE_CMD_PORT, PIC_EOI); // slave because we are above 0x20
}

void send_packet(uint8_t dst_hw_addr[], uint8_t* data, uint32_t length) {
	struct cb* cb = (struct cb*) dumb_malloc(sizeof(struct cb));

	(void) dst_hw_addr;
	(void) data;
	(void) length;
	// 
	// fill 'er up with juicy bits
	// 

	execute_command(cb);
}

void execute_command(struct cb* cb) {
	(void) cb;
	if(_nic.csr->scb.status & (cu_lpq_active | cu_hqp_active)) { // CU active
		// Add pointer to new CB to tail of CBL

	}
	else if(_nic.csr->scb.status & cu_idle) { // CU idle
		// CU start

	}
	else if(_nic.csr->scb.status & cu_suspended) { // CU suspended
		// Add pointer to new CB to tail of CBL, then CU resume

	}
	else {
		// ?!?!? Famous last words, "this can never happen"
	}
	// Craft a beautiful Command Block
	// Check where we should put this command block 
	// --- keep a pointer to the last CB for chaining if CU is running
	// --- Run CU Resume if CU is idle
	// --- if CU is active put CB ptr into gen_ptr and run CU Start
}

void intel_nic_init() {
	uint8_t bus, slot;
	// Try to find DSL card first, followed by QEMU card, then give up
	if(pci_get_device(&bus, &slot, NET_INTEL_VENDOR, NET_INTEL_DSL_NIC) >= 0) {	
		c_printf("DSL Lab Intel NIC found - bus: %d, slot: %d\n", bus, slot);
	}
	else if(pci_get_device(&bus, &slot, NET_INTEL_VENDOR, NET_INTEL_QEMU_NIC) >= 0) {
		c_printf("QEMU Intel NIC found - bus: %d, slot: %d\n", bus, slot);
	}
	else {
		c_printf("Could not find Intel NIC\n");
		return;
	}

	// c_printf("wait a few seconds to read PCI info... ");
	// int number = 0;
	// for (int i = 0; i < 1000000; ++i)
	// {
	// 	if(i % 100000) {
	// 		c_printf(">");
	// 	}
	// 	number += 5;
	// 	/* code */
	// }
	// c_printf("done waiting.\n");

	_nic.csr = (struct csr *) pci_cfg_read(bus, slot, 0, PCI_CSR_MEM_MAPPED_BASE_ADDR_REG);

	c_printf("CSR MMIO base addr: 0x%08x\n", (uint32_t) _nic.csr);

	if(_nic.csr == 0) {
		__panic("PCI READ FAILURE");
	}

	c_printf("Loading data from EEPROM...\n");
	eeprom_load(&_nic);

	_nic.mac[0] = (uint8_t) (_nic.eeprom[0] & 0x00FF);
	_nic.mac[1] = (uint8_t) (_nic.eeprom[0] >> 8);
	_nic.mac[2] = (uint8_t) (_nic.eeprom[1] & 0x00FF);
	_nic.mac[3] = (uint8_t) (_nic.eeprom[1] >> 8);
	_nic.mac[4] = (uint8_t) (_nic.eeprom[2] & 0x00FF);
	_nic.mac[5] = (uint8_t) (_nic.eeprom[2] >> 8);
	c_printf("MAC Address: ");
	print_mac_addr(_nic.mac);
	c_printf("\n");

	// set base CU to 0
	mem_write32(&_nic.csr->scb.gen_ptr, 0);
	mem_write8(&_nic.csr->scb.command, cuc_load_cu_base);
	write_flush(&_nic);

	// connect interrupt handler
	// uint8_t interrupt_pin = pci_cfg_read_byte(bus, slot, 0, PCI_INTERRUPT_PIN); // showing up as 0x01
	// uint8_t interrupt_line = pci_cfg_read_byte(bus, slot, 0, PCI_INTERRUPT_LINE); // showing up as 0x0B
	// c_printf("interrupt pin = 0x%02x\ninterrupt line = 0x%02x\n", interrupt_pin, interrupt_line);
	// __install_isr(interrupt_line + 0x20, intel_nic_handler);
	__install_isr(0x0B + 0x20, intel_nic_handler);


	// struct cb* cb_ia = (struct cb*) dumb_malloc(sizeof(struct cb));
	// cb_ia->command = cb_ia_cmd;
	// cb_ia->link = (uint32_t) cbl_ptr;
	// memcpy(cb_ia->u.mac_addr, _nic.mac, MAC_LENGTH_BYTES);
	// c_printf("setting IA to: ");
	// print_mac_addr(cb_ia->u.mac_addr);


	//
	// init singly-linked CB ring
	//
	// c_printf("Initializing CB ring\n");
	// struct cb* first = (struct cb*) dumb_malloc(sizeof(struct cb));
	// struct cb* prev = first;
	// for(uint32_t i = 0; i < TOTAL_CB; i++) {
	// 	prev->link = (uint32_t) dumb_malloc(sizeof(struct cb));
	// 	prev = (struct cb*) prev->link;
	// }
	// prev->link = (uint32_t) first; // complete the circle


	// page 83 of manual
	c_printf("Sending Tx CB\n");
	// struct cb CB; struct cb* cbl_ptr = &CB;
	struct cb* cbl_ptr = (struct cb*) dumb_malloc(sizeof(struct cb));
	cbl_ptr->command = cb_el | cb_sf | cb_tx_cmd; // end of cbl, simplified mode, transmit

	cbl_ptr->u.tcb.tbd_array = 0xFFFFFFFF; // for simplified mode, tbd is 1's, data is in tcb
	cbl_ptr->u.tcb.tcb_byte_count = 0x48 | 0x8000; // 0x8000 = EOF flag
	cbl_ptr->u.tcb.threshold = 1; // transmit once you have (1 * 8 bytes) in the queue
	cbl_ptr->u.tcb.tbd_count = 0;

	cbl_ptr->u.tcb.eth_header.dst_mac[0] = 0x00;
	cbl_ptr->u.tcb.eth_header.dst_mac[1] = 0xE0;
	cbl_ptr->u.tcb.eth_header.dst_mac[2] = 0x7C;
	cbl_ptr->u.tcb.eth_header.dst_mac[3] = 0xC8;
	cbl_ptr->u.tcb.eth_header.dst_mac[4] = 0x7D;
	cbl_ptr->u.tcb.eth_header.dst_mac[5] = 0x08;
	memcpy(cbl_ptr->u.tcb.eth_header.src_mac, _nic.mac, MAC_LENGTH_BYTES);

	cbl_ptr->u.tcb.eth_header.ethertype_lo = 0x40;

	cbl_ptr->u.tcb.eth_header.data[0x00] = 'h';
	cbl_ptr->u.tcb.eth_header.data[0x01] = 'e';
	cbl_ptr->u.tcb.eth_header.data[0x02] = 'l';
	cbl_ptr->u.tcb.eth_header.data[0x03] = 'l';
	cbl_ptr->u.tcb.eth_header.data[0x04] = 'o';
	cbl_ptr->u.tcb.eth_header.data[0x05] = ' ';
	cbl_ptr->u.tcb.eth_header.data[0x06] = 'w';
	cbl_ptr->u.tcb.eth_header.data[0x07] = 'o';
	cbl_ptr->u.tcb.eth_header.data[0x08] = 'r';
	cbl_ptr->u.tcb.eth_header.data[0x09] = 'l';
	cbl_ptr->u.tcb.eth_header.data[0x0A] = 'd';
	cbl_ptr->u.tcb.eth_header.data[0x0B] = '!';
	cbl_ptr->u.tcb.eth_header.data[0x0C] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x0D] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x0E] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x0F] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x10] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x11] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x12] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x13] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x14] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x15] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x16] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x17] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x18] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x19] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x1A] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x1B] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x1C] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x1D] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x1E] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x1F] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x20] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x21] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x22] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x23] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x24] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x25] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x26] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x27] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x28] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x29] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x2A] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x2B] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x2C] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x2D] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x2E] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x2F] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x30] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x31] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x32] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x33] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x34] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x35] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x36] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x37] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x38] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x39] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x3A] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x3B] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x3C] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x3D] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x3E] = 0x42;
	cbl_ptr->u.tcb.eth_header.data[0x3F] = 0x42;

	// give the cbl addr to the CU and start
	c_printf("Putting CBL pointer into gen_ptr...\n");
	mem_write32(&_nic.csr->scb.gen_ptr, (uint32_t) cbl_ptr);
	c_printf("calling CU Start on CBL...\n");
	mem_write8(&_nic.csr->scb.command, cuc_start);
	c_printf("flushing output...\n");
	write_flush(&_nic);
	c_printf("Tx CU finished\n");

	// 
	// 
	// TODO:
	// o Keep ring of available CB linked together, set EL flag on last one, but still have the links there
	// o Finish writing ISR
	// o Write send_packet function
	// o Write execute_command function
	// o Figure out what should stay static in intel.c and if some internal stuff should be removed from intel.h
	// o configure receive buffers
	// o enable receiving data
	// o Write routine to output configure command blocks
	// o mutex on doing anything with CB
	// 
	// --Merge memory stuff in from master
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
