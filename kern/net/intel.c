#include <kern/net/intel.h>
#include <kern/pci/pci.h>
#include <kern/memory/memory_map.h> //get_next_page, free_page

#include <baseline/c_io.h> //c_printf
#include <string.h> //memcpy
#include <x86arch.h> //PIC_EOI, PIC_SLAVE_CMD_PORT
#include <baseline/support.h> //__install_isr
#include <baseline/startup.h> //__outb


#define TRUE 1
#define FALSE 0

//
// Static method declarations
//
static void* dumb_malloc(uint32_t size) __attribute__((unused));
static void delay_10usec(uint32_t usec_10);
static void print_mac_addr(uint8_t mac[]);
static void dump_eeprom(struct nic_info* nic) __attribute__((unused));
static void write_flush(struct nic_info *nic);
static uint16_t eeprom_read(struct nic_info *nic, uint16_t *addr_len, uint16_t addr);
static void eeprom_load(struct nic_info *nic);
static void intel_nic_handler(int vector, int code);
static void init_cbl(struct nic_info* nic, uint32_t num_cb);
static void init_rfa(struct nic_info* nic, uint32_t num_rfd);
static void recycle_command_blocks();
static void claim_rfd_data();
static struct cb* get_next_cb();
static int32_t send_arp();


static uint32_t mem_read32(void* addr) __attribute__((unused));
static uint16_t mem_read16(void* addr) __attribute__((unused));
static uint8_t mem_read8(void* addr);
static void mem_write32(void* addr, uint32_t value);
static void mem_write16(void* addr, uint16_t value) __attribute__((unused));
static void mem_write8(void* addr, uint8_t value);

// Holds info about the network interface
static struct nic_info _nic;

static uint32_t base_free = 0x3A00; // this seems safe...
static uint32_t free_top = 0x7BFF;

static void* dumb_malloc(uint32_t size) {
	if(base_free + size > free_top) {
		return NULL;
	}
	void* ret = (void*) base_free;
	base_free += size;
	return ret;
}

/**
 * Delay for 10 microseconds
 *
 * @param usec_10 length to delay
 */
static void delay_10usec(uint32_t usec_10) {
	for (; usec_10 > 0; usec_10--) {
		for (uint32_t j = 0; j < 4000; ++j) { } // 10us
	}
}

/**
 * Prints out a MAC address in XX:XX:XX:XX:XX:XX notation
 *
 * @param mac uint8_t[6] containing MAC address
 */
static void print_mac_addr(uint8_t mac[]) {
	c_printf("%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

/**
 * Prints out the contents of the in-memory EEPROM cache
 *
 * @param nic where to get EEPROM from
 */
static void dump_eeprom(struct nic_info* nic) {
	for(uint32_t i = 0; i < nic->eeprom_count; i++) {
		if(i % 6 == 0)
			c_printf("\neep:");
		c_printf(" [%d]=0x%04x", i, nic->eeprom[i]);
	}
}

/**
 * Flushes the MMIO struct to the device by reading in something innocuous
 *
 * @param nic card to flush for
 */
static void write_flush(struct nic_info *nic) {
	(void) mem_read8(&nic->csr->scb.status);
}

/**
 * Reads from the EEPROM
 * Inspired by linux kernel: linux/drivers/net/ethernet/intel/e100.c
 *
 * @param nic location to store the EEPROM data
 * @param addr_len 
 * @param addr
 * @return
 */
static uint16_t eeprom_read(struct nic_info *nic, uint16_t *addr_len, uint16_t addr) {
	uint32_t cmd_addr_data;
	uint16_t data = 0;
	uint8_t ctrl;
	int32_t i;

	cmd_addr_data = ((op_read << *addr_len) | addr) << 16;
	/* Chip select */
	mem_write8(&nic->csr->eeprom_lo, EECS | EESK);

	write_flush(nic); delay_10usec(1);
	
	/* Bit-bang to read word from eeprom */
	for (i = 31; i >= 0; i--) {
		ctrl = (cmd_addr_data & (1 << i)) ? EECS | EEDI : EECS;
		mem_write8(&nic->csr->eeprom_lo, ctrl);
		write_flush(nic); delay_10usec(1);

		mem_write8(&nic->csr->eeprom_lo, ctrl | EESK);
		write_flush(nic); delay_10usec(1);

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
	write_flush(nic); delay_10usec(1);
	return data;
}

/**
 * Loads the entire EEPROM into memory cache
 * Inspired by linux kernel: linux/drivers/net/ethernet/intel/e100.c
 * 
 * @param nic specify where EEPROM cache should be stored
 */
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
	c_printf("\n");

	/* The checksum, stored in the last word, is calculated such that
	 * the sum of words should be 0xBABA */
	// if((0xBABA - checksum) != nic->eeprom[nic->eeprom_count - 1]) {
	// 	c_printf("~~~ EEPROM corrupt ~~~\n");
	// }
}


/**
 * Interrupt handler for the NIC
 *
 * @param vector vector for the interrupt
 * @param code code for the interrupt
 */
static void intel_nic_handler(int vector, int code) {
	(void) vector; (void) code;
	c_printf("INTERRUPT(v=0x%02x, c=0x%02x) -- Intel NIC, ", vector, code);
	uint8_t stat_ack = mem_read8(&_nic.csr->scb.stat_ack);
	c_printf("stat_ack=0x%02x ", stat_ack);

	if(stat_ack & ack_cs_tno) c_printf("ack_cs_tno ");
	if(stat_ack & ack_fr) {
		c_printf("ack_fr ");
		// claim_rfd_data();
	}
	if(stat_ack & ack_cna) c_printf("ack_cna ");
	if(stat_ack & ack_rnr) c_printf("ack_rnr ");
	if(stat_ack & ack_mdi) c_printf("ack_mdi ");
	if(stat_ack & ack_swi) c_printf("ack_swi ");
	c_printf("\n");

	recycle_command_blocks();

	// acknowledge status bits
	mem_write8(&_nic.csr->scb.stat_ack, ~0);
	write_flush(&_nic);

	if(vector >= 0x20 && vector < 0x30) {
		__outb(PIC_MASTER_CMD_PORT, PIC_EOI);
		if(vector > 0x27) {
			__outb(PIC_SLAVE_CMD_PORT, PIC_EOI);
		}
	}
}

/**
 * Initialize the NIC with a ring of command blocks
 *
 * @param nic card to initialize the ring for
 * @param num_cb number of command blocks to create
 */
static void init_cbl(struct nic_info* nic, uint32_t num_cb) {
	struct cb* first = (struct cb*) get_next_page();
	struct cb* prev = first;
	for(uint32_t i = 0; i < num_cb; i++) {
		prev->link = (uint32_t) get_next_page();
		prev = (struct cb*) prev->link;
	}
	prev->link = (uint32_t) first; // complete the circle

	//put this stuff into the nic_info
	nic->avail_cb = num_cb;
	nic->next_cb = first;
	nic->cb_to_check = first;
}

/**
 * Initialize the NIC with a ring of receive frame descriptors
 *
 * @param nic card to initialize the ring for
 * @param num_rfd number of receive frame descriptors to create
 */
static void init_rfa(struct nic_info* nic, uint32_t num_rfd) {
	struct rfd* first = (struct rfd*) get_next_page();
	struct rfd* curr = first;
	curr->size = NET_INTEL_RFD_SIZE;
	curr->command = 1 << 3; // simplified mode
	for(uint32_t i = 0; i < num_rfd; i++) {
		curr->link = (uint32_t) get_next_page();
		curr = (struct rfd*) curr->link;
		curr->size = NET_INTEL_RFD_SIZE;
		curr->command = 1 << 3; // simplified mode
	}
	curr->link = (uint32_t) first; // complete the circle
	nic->next_rfd = first;
}

/**
 * frees up command blocks (and associated buffers) for future use
 */
static void recycle_command_blocks() {
	c_printf("NIC: attempting to free up some command blocks");
	while(_nic.cb_to_check->status & cb_c) {
		c_printf("NIC: freeing command block: %08x\n", (uint32_t) _nic.cb_to_check);
		_nic.cb_to_check->status &= ~cb_c; // unset complete flag
		_nic.cb_to_check->command &= 0;
		_nic.avail_cb++; // increment available command blocks
		_nic.cb_to_check = (struct cb*) _nic.cb_to_check->link;
	}
}

// static void claim_rfd_data() {
// 	struct rfd* rfd = _nic.next_rfd;
// 	uint16_t byte_count = rfd->count & 0x3FFF;
// 	c_printf("EOF=%c,F=%c,received %d bytes\n", 
// 		(rfd->count & 0x8000) ? '1' : '0', 
// 		(rfd->count & 0x4000) ? '1' : '0', 
// 		rfd->count & 0x3FFF);
// 	for(uint16_t i = 0; i < byte_count; i+=4) {
// 		c_printf("[%4d] = %02x%02x %02x%02x (%c%c%c%c)\n",
// 			i,
// 			rfd->data[i],
// 			rfd->data[i + 1],
// 			rfd->data[i + 2],
// 			rfd->data[i + 3],
// 			rfd->data[i],
// 			rfd->data[i + 1],
// 			rfd->data[i + 2],
// 			rfd->data[i + 3]);
// 	}
// 	// TODO finish writing this
// 	// 
// 	// check _nic.next_rfd to see if its been written to
// 	// if it has:
// 	//     process data (read actual count)
// 	//     clear EOF and F flags
// 	//     
// 	//     _nic.next_rfd = (struct rfd*) _nic.next_rfd->link;
// 	// 
// }

/**
 * Gets the next command block from the nic_info
 *
 * @return next available command block, NULL if there aren't any
 */
static struct cb* get_next_cb() {
	if(!_nic.avail_cb) {
		recycle_command_blocks();
		if(!_nic.avail_cb) {
			c_printf("NIC: No available Command Blocks :(\n");
			return NULL;
		}
	}
	struct cb* cb = _nic.next_cb;
	_nic.next_cb = (struct cb*) cb->link;
	_nic.avail_cb--;
	return cb;
}

// static int32_t send_arp() {
// 	struct cb* cb = get_next_cb();
// 	if(cb == NULL) {
// 		return -1;
// 	}
// 	struct arp arp; // TODO network byte order?
// 	arp.hw_type = 0x0001; // ethernet
// 	arp.protocol_type = 0x0800; // IPv4
// 	arp.hw_addr_len = 0x06; // ethernet HW addr length
// 	arp.protocol_addr_len = 0x04; // IPv4 addr length
// 	arp.opcode = 0x01; // request
// 	memcpy(arp.sender_hw_addr, _nic.mac, MAC_LENGTH_BYTES);
// 	arp.sender_protocol_addr = 0xC4C4C4C4;
// 	arp.target_hw_addr[0] = 0xFF;
// 	arp.target_hw_addr[1] = 0xFF;
// 	arp.target_hw_addr[2] = 0xFF;
// 	arp.target_hw_addr[3] = 0xFF;
// 	arp.target_hw_addr[4] = 0xFF;
// 	arp.target_hw_addr[5] = 0xFF;
// 	arp.target_protocol_addr = 0x00000000;

// 	// TODO attach these things together
// 	// ethertype = 0x0806 for ARP
// }

int32_t send_packet(uint8_t dst_hw_addr[], void* data, uint32_t length) {
	if(length > NET_INTEL_MAX_ETH_LENGTH) {
		return -1;
	}

	struct cb* cb = get_next_cb();
	if(cb == NULL) {
		return -1;
	}

	uint32_t tx_length = (length < NET_INTEL_MIN_ETH_LENGTH) ? NET_INTEL_MIN_ETH_LENGTH : length;
	// ensure minimum frame size is met by padding from length of user data
	// through 46 bytes
	for (int i = length; i <= NET_INTEL_MIN_ETH_LENGTH; i++) {
		cb->u.tcb.eth_packet.data[i] = 0;
	}

	//
	// Simplified mode
	//
	cb->command = cb_el | cb_sf | cb_tx_cmd;
	cb->u.tcb.tbd_array = 0xFFFFFFFF; // for simplified mode, tbd is 1's, data is in tcb
	cb->u.tcb.tcb_byte_count = (tx_length + NET_INTEL_ETH_HEAD_LEN)| 1 << 15; // bit15 = EOF flag
	cb->u.tcb.threshold = 1; // transmit once you have (1 * 8 bytes) in the queue
	cb->u.tcb.tbd_count = 0;
	memcpy(cb->u.tcb.eth_packet.dst_mac, dst_hw_addr, MAC_LENGTH_BYTES);

	cb->u.tcb.eth_packet.ethertype = ((tx_length >> 8) & 0x00FF) | (((tx_length << 8) & 0xFF00)); // network byte order
	memcpy(cb->u.tcb.eth_packet.data, data, length);

	uint8_t status = mem_read8(&_nic.csr->scb.status);
	c_printf("status: 0x%02x\n", status);
	if(((status & cu_mask) == cu_idle) 
			|| ((status & cu_mask) == cu_suspended)) {
		c_printf("NIC: Starting CUC\n");
		mem_write32(&_nic.csr->scb.gen_ptr, (uint32_t) cb);
		mem_write8(&_nic.csr->scb.command, cuc_start);
		write_flush(&_nic);
	}
	return 0;
}

void intel_nic_init() {
	// Try to find DSL card first, followed by QEMU card, then give up
	uint8_t bus, slot;
	if(pci_get_device(&bus, &slot, NET_INTEL_VENDOR, NET_INTEL_DSL_NIC) >= 0) {	
		c_printf("NIC: Intel 8255x found - bus: %d, slot: %d\n", bus, slot);
	}
	else {
		c_printf("NIC: Could not find Intel NIC\n");
		return;
	}

	_nic.csr = (struct csr *) pci_cfg_read(bus, slot, 0, PCI_CSR_MEM_MAPPED_BASE_ADDR_REG);
	c_printf("NIC: CSR MMIO base addr: 0x%08x\n", (uint32_t) _nic.csr);

	c_printf("NIC: Loading data from EEPROM...\n");
	eeprom_load(&_nic);

	//
	// MAC address setup
	//
	_nic.mac[0] = (uint8_t) (_nic.eeprom[0] & 0x00FF);
	_nic.mac[1] = (uint8_t) (_nic.eeprom[0] >> 8);
	_nic.mac[2] = (uint8_t) (_nic.eeprom[1] & 0x00FF);
	_nic.mac[3] = (uint8_t) (_nic.eeprom[1] >> 8);
	_nic.mac[4] = (uint8_t) (_nic.eeprom[2] & 0x00FF);
	_nic.mac[5] = (uint8_t) (_nic.eeprom[2] >> 8);
	c_printf("NIC: HW MAC Address: ");
	print_mac_addr(_nic.mac);
	c_printf("\n");

	//
	// Set base CU and RU to 0
	//
	mem_write32(&_nic.csr->scb.gen_ptr, 0);
	mem_write8(&_nic.csr->scb.command, cuc_load_cu_base);
	write_flush(&_nic);
	mem_write32(&_nic.csr->scb.gen_ptr, 0);
	mem_write8(&_nic.csr->scb.command, ruc_load_ru_base);
	write_flush(&_nic);

	// 
	// Connect interrupt handler
	// 
	uint8_t interrupt_pin = pci_cfg_read_byte(bus, slot, 0, PCI_INTERRUPT_PIN); // showing up as 0x01
	uint8_t interrupt_line = pci_cfg_read_byte(bus, slot, 0, PCI_INTERRUPT_LINE); // showing up as 0x0B
	c_printf("NIC: interrupt pin = 0x%02x\ninterrupt line = 0x%02x\n", interrupt_pin, interrupt_line);
	__install_isr(NET_INTEL_INT_VECTOR, intel_nic_handler);

	// 
	// Setup the CBL and RFA
	// 
	c_printf("NIC: Initializing CBL\n");
	init_cbl(&_nic, TOTAL_CB);
	// c_printf("NIC: Initializing RFA\n");
	// init_rfa(&_nic, TOTAL_RFD);


	//
	// Initial Configure command
	// 
	c_printf("NIC: Sending initial configure command");
	struct cb* configure_cb = get_next_cb();
	configure_cb->command = cb_cfg_cmd;
	configure_cb->u.cfg[0] = 16; // 16 bytes of configuration
	configure_cb->u.cfg[1] = 8;
	configure_cb->u.cfg[2] = 0;
	configure_cb->u.cfg[3] = 0;
	configure_cb->u.cfg[4] = 0;
	configure_cb->u.cfg[5] = 0;
	configure_cb->u.cfg[6] = 
		1 << 7 |
		1 << 6 |
		1 << 5 |
		1 << 4 |
		1 << 2 |
		1 << 1;
	configure_cb->u.cfg[7] = 1 << 1;
	configure_cb->u.cfg[8] = 0;
	configure_cb->u.cfg[9] = 0; 
	configure_cb->u.cfg[10] = 
		1 << 5 | // 7 byte ethernet preamble
		1 << 2 |
		1 << 1;
	configure_cb->u.cfg[11] = 0;
	configure_cb->u.cfg[12] = 6 << 4;
	configure_cb->u.cfg[13] = 0x00;
	configure_cb->u.cfg[14] = 0xF2;
	configure_cb->u.cfg[15] = 
		1 << 7 | 
		1 << 6 | 
		1 << 3 | 
		1; // promiscuous mode


	// 
	// Individual Address Configuration
	// 
	struct cb* ia_cb = get_next_cb();
	ia_cb->command = cb_ia_cmd | cb_el;
	memcpy(ia_cb->u.mac_addr, _nic.mac, MAC_LENGTH_BYTES);
	c_printf("NIC: Setting IA to: ");
	print_mac_addr(ia_cb->u.mac_addr);
	c_printf("\n");

	// [configure_cb]-->[ia_cb]-->
	mem_write32(&_nic.csr->scb.gen_ptr, (uint32_t) configure_cb);
	mem_write8(&_nic.csr->scb.command, cuc_start);
	write_flush(&_nic);

	//
	// Enable Receiving
	//
	// mem_write32(&_nic.csr->scb.gen_ptr, (uint32_t) _nic.next_rfd);
	// mem_write8(&_nic.csr->scb.command, ruc_start);

	//
	// ARP
	//
	// send_arp();

	// 
	// 
	// TODO:
	// o handle receive interrupts in addition to CU interrupts
	// o write raw_tcb_tx()
	// o finish send_arp(), and handle arp packets
	// o IPv4 header insertion
	// o mutex on doing anything with CB
	// 
	// TEST:
	// o enable receiving data by putting first RFD ptr into gen_ptr (pg 99)
	// o configure receive buffers
	// 
	// DONE:
	// o make sure CRC is being inserted (ask other networking guy)
	// o Write send_packet function (check on ethernet header)
	// o configure IA
	// o change configure command to make sure NSAI (byte 10) is set correctly to insert source address
	// o Write routine to output configure command blocks
	// o figure out if interrupt handler needs to send EOI (caused bugs before)
	// o ensure CBL ring and cleanup actually works
	// o Finish writing ISR (cleanup CB)
	// o Figure out what should stay static in intel.c and if some internal stuff should be removed from intel.h
	// o Keep ring of available CB linked together, set EL flag on last one, but still have the links there
	// o Merge memory stuff in from master, HINT MERGE NOT REBASE
	// 
	// 

}

static uint32_t mem_read32(void* addr) {
	return *(volatile uint32_t*) addr;
}

static uint16_t mem_read16(void* addr) {
	return *(volatile uint16_t*) addr;
}

static uint8_t mem_read8(void* addr) {
	return *(volatile uint8_t*) addr;
}

static void mem_write32(void* addr, uint32_t value) {
	*(volatile uint32_t*) addr = value;
}

static void mem_write16(void* addr, uint16_t value) {
	*(volatile uint16_t*) addr = value;
}

static void mem_write8(void* addr, uint8_t value) {
	*(volatile uint8_t*) addr = value;
}
