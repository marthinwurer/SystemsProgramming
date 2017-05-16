#include <kern/net/intel.h>
#include <kern/pci/pci.h>
#include <kern/memory/memory_map.h> //get_next_page, free_page

#include <baseline/c_io.h> //c_printf
#include <string.h> //memcpy
#include <x86arch.h> //PIC_EOI, PIC_SLAVE_CMD_PORT
#include <baseline/support.h> //__install_isr
#include <baseline/startup.h> //__outb

/**
 * Network driver for the Intel i82557. 
 *
 * 
 * @author Daniel Meiller
 */

//
// Static method declarations
//
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
static char ascii_to_printable_char(char c);
static rfd_t* make_new_tail();
static struct cb* get_next_cb();
static uint16_t ip_chksm(void* ip_head, int len);
static int32_t save_hw_addr(uint32_t ip, uint8_t hw_addr[]);
static int32_t get_hw_addr(uint32_t ip, uint8_t hw_addr_out[]);
static int32_t send_grat_arp(uint32_t sender_ip_addr);
static int32_t send_arp_reply(uint32_t target_ip_addr, uint8_t target_hw_addr[]);
static int32_t send_arp_request(uint32_t target_ip_addr);
static int32_t send_arp(uint32_t sender_ip_addr, uint32_t target_ip_addr, uint8_t target_hw_addr[], enum arp_opcode opcode);

static uint32_t mem_read32(void* addr) __attribute__((unused));
static uint16_t mem_read16(void* addr) __attribute__((unused));
static uint8_t mem_read8(void* addr);
static void mem_write32(void* addr, uint32_t value);
static void mem_write16(void* addr, uint16_t value) __attribute__((unused));
static void mem_write8(void* addr, uint8_t value);

// Holds info about the network interface
static struct nic_info _nic;

// hold ip->mac entries
static arp_entry_t* _arp_cache;

// static uint32_t base_free = 0x3A00; // this seems safe...
// static uint32_t free_top = 0x7BFF;

// static void* dumb_malloc(uint32_t size) {
// 	if(base_free + size > free_top) {
// 		return NULL;
// 	}
// 	void* ret = (void*) base_free;
// 	base_free += size;
// 	return ret;
// }

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
	// c_printf("INTERRUPT(v=0x%02x, c=0x%02x) -- Intel NIC, ", vector, code);
	uint8_t stat_ack = mem_read8(&_nic.csr->scb.stat_ack);
	// c_printf("stat_ack=0x%02x ", stat_ack);

	// if(stat_ack & ack_cs_tno) c_printf("ack_cs_tno ");
	if(stat_ack & ack_fr) {
		rfd_t* rfd = _nic.rfa.next;
		rfd->count &= ~0x8000; //clear EOF
		rfd->count &= ~0x4000; //clear F
		_nic.rfa.next = (rfd_t*) rfd->link;
	}
	// if(stat_ack & ack_cna) c_printf("ack_cna ");
	// if(stat_ack & ack_rnr) c_printf("ack_rnr ");
	// if(stat_ack & ack_mdi) c_printf("ack_mdi ");
	// if(stat_ack & ack_swi) c_printf("ack_swi ");
	// c_printf("\n");

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
	struct cb* curr = first;
	curr->status = 0;
	for(uint32_t i = 0; i < num_cb; i++) {
		curr->link = (uint32_t) get_next_page();
		curr = (struct cb*) curr->link;
		curr->status = 0;
	}
	curr->link = (uint32_t) first; // complete the circle

	nic->avail_cb = num_cb;
	nic->next_cb = first;
	nic->cb_to_check = first;
}

/**
 * Initialize the NIC with a singly linked list of receive frame descriptors
 * head=curr                                 tail
 *  [] ---> [] ---> [] ---> [] ---> [] ---> [EL=1]
 * @param nic card to initialize the ring for
 * @param num_rfd number of receive frame descriptors to create
 */
static void init_rfa(struct nic_info* nic, uint32_t num_rfd) {
	rfd_t* curr = (rfd_t*) get_next_page();
	nic->rfa.head = curr;
	nic->rfa.next = curr;
	for(uint32_t i = 0; i < num_rfd; i++) {
		memset(curr, 0, 16); // zero out the RFD header
		curr->size = NET_INTEL_RFD_SIZE;
		curr->command = 1 << 3; // simplified mode
		
		curr->link = (uint32_t) get_next_page();
		curr = (rfd_t*) curr->link;
	}
	memset(curr, 0, 16); // zero out the RFD header
	curr->size = NET_INTEL_RFD_SIZE;
	curr->command = 0x8000 | (1 << 3); // last in the list and simplified
	nic->rfa.tail = curr;
}

/**
 * frees up command blocks (and associated buffers) for future use
 */
static void recycle_command_blocks() {
	while(_nic.cb_to_check->status & cb_c) {
		// c_printf("NIC: freeing command block: %08x\n", (uint32_t) _nic.cb_to_check);
		_nic.cb_to_check->status &= ~cb_c; // unset complete flag
		_nic.cb_to_check->command &= 0;
		_nic.avail_cb++; // increment available command blocks
		_nic.cb_to_check = (struct cb*) _nic.cb_to_check->link;
	}
}

/**
 * convert a char to an always printable character
 *
 * @param c char to convert
 * @return printable char
 */
static char ascii_to_printable_char(char c) {
	if(c  > 31 && c < 127) {
		return c;
	}
	else {
		return '.';
	}
}

/**
 * Create a new tail for the RFA list
 *
 * @return new tail for the list
 */
static rfd_t* make_new_tail() {
	rfd_t* curr = (rfd_t*) get_next_page();
	memset(curr, 0, 16); // zero out the RFD header
	curr->size = NET_INTEL_RFD_SIZE;
	curr->command = 0x8000 | (1 << 3);
	return curr;
}

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
	// c_printf("NIC: get_next_cb = 0x%08x\n", (uint32_t) cb);
	_nic.next_cb = (struct cb*) cb->link;
	_nic.avail_cb--;
	return cb;
}

/**
 * Sends a gratuitous ARP packet with a given IP
 *
 * @param sender_ip_addr sender's IP address
 * @return 0 on success, otherwise failure
 */
static int32_t send_grat_arp(uint32_t sender_ip_addr) {
	uint8_t target_hw_addr[6];
	target_hw_addr[0] = 0xFF;
	target_hw_addr[1] = 0xFF;
	target_hw_addr[2] = 0xFF;
	target_hw_addr[3] = 0xFF;
	target_hw_addr[4] = 0xFF;
	target_hw_addr[5] = 0xFF;
	return send_arp(sender_ip_addr, sender_ip_addr, target_hw_addr, arp_request);
}

/**
 * Sends an ARP reply
 *
 * @param target_ip_addr IP to send ARP reply to
 * @param target_hw_addr HW addr to send ARP reply to
 * @return
 */
static int32_t send_arp_reply(uint32_t target_ip_addr, uint8_t target_hw_addr[]) {
	return send_arp(_nic.my_ip, target_ip_addr, target_hw_addr, arp_reply);
}

/**
 * Sends an ARP request
 *
 * @param target_ip_addr who to send the ARP request to
 * @return
 */
static int32_t send_arp_request(uint32_t target_ip_addr) {
	c_printf("Sending arp request\n");
	uint8_t zero_mac[6] = {0,0,0,0,0,0};
	return send_arp(_nic.my_ip, target_ip_addr, zero_mac, arp_request);
}

/**
 * Sends an ARP packet
 *
 * @param sender_ip_addr sender's IP address
 * @param target_ip_addr target's IP address
 * @param target_hw_addr target's HW address
 * @param opcode type of arp (request/reply)
 * @return 0 on success, otherwise failure
 */
static int32_t send_arp(uint32_t sender_ip_addr, uint32_t target_ip_addr, uint8_t target_hw_addr[], enum arp_opcode opcode) {
	arp_t arp;
	arp.hw_type = 0x0100; // ethernet in NBO
	arp.protocol_type = 0x0008; // IPv4 inn NBO
	arp.hw_addr_len = 0x06; // ethernet HW addr length
	arp.protocol_addr_len = 0x04; // IPv4 addr length
	arp.opcode = opcode; // request in NBO

	memcpy(&arp.sender_hw_addr, _nic.mac, MAC_LENGTH_BYTES);
	arp.sender_protocol_addr = __builtin_bswap32(sender_ip_addr);
	arp.target_protocol_addr = __builtin_bswap32(target_ip_addr);
	// memcpy(&arp.sender_protocol_addr, sender_ip_addr, 4);
	// memcpy(&arp.target_protocol_addr, target_ip_addr, 4);
	memcpy(&arp.target_hw_addr, target_hw_addr, MAC_LENGTH_BYTES);
	send_packet(target_hw_addr, &arp, sizeof(arp_t), ethertype_arp);
	return 0;
}


int32_t send_packet(uint8_t dst_hw_addr[], void* data, uint16_t length, ethertype_t ethertype) {
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
		cb->u.tcb.eth_packet.payload.data[i] = 0;
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

	cb->u.tcb.eth_packet.ethertype = ethertype;
	// cb->u.tcb.eth_packet.ethertype = ((tx_length >> 8) & 0x00FF) | (((tx_length << 8) & 0xFF00)); // network byte order
	memcpy(cb->u.tcb.eth_packet.payload.data, data, length);
	// c_printf("offset for ethernet payload data: 0x%08x\n",(uint32_t) &cb->u.tcb.eth_packet.payload.data - (uint32_t) &cb->u.tcb.eth_packet);

	uint8_t status = mem_read8(&_nic.csr->scb.status);
	// c_printf("NIC: send_packet CU/RU status = 0x%02x\n", status);
	if(((status & cu_mask) == cu_idle) 
			|| ((status & cu_mask) == cu_suspended)) {
		mem_write32(&_nic.csr->scb.gen_ptr, (uint32_t) cb);
		mem_write8(&_nic.csr->scb.command, cuc_start);
		write_flush(&_nic);
	}
	return 0;
}

static uint16_t ip_chksm(void* ip_head, int len) {
	uint32_t sum = 0;
	const uint16_t *ip1 = ip_head;
	while(len > 1){
		// sum += *((uint16_t*) ip_head)++;
		sum += *ip1++;
		if(sum & 0x80000000) {
			sum = (sum & 0xFFFF) + (sum >> 16);
		}
		len -= 2;
	}
	if(len) {
		sum += (uint16_t) *(uint8_t*) ip_head;
	}
	while(sum >> 16) {
		sum = (sum & 0xFFFF) + (sum >> 16);
	}
	return ~sum;
}

static int32_t save_hw_addr(uint32_t ip, uint8_t hw_addr[]) {
	uint32_t hash = ip % ARP_CACHE_SIZE;
	for(int index = hash, count = 0; count < ARP_CACHE_SIZE; index++, count++) {
		if(index == ARP_CACHE_SIZE) index = 0; // wrap around
		// c_printf("trying to insert 0x%08x at _arp_cache[%d]\n", ip, index);
		if(_arp_cache[index].ip_addr == ip || !_arp_cache[index].filled) { // update or add
			// c_printf("inserting at _arp_cache[%d]\n", index);
			_arp_cache[index].ip_addr = ip;
			_arp_cache[index].filled = 1;
			memcpy(_arp_cache[index].hw_addr, hw_addr, 6);
			return 0;
		}
	}
	return -1;
}

/**
 * [get_hw_addr description]
 *
 * @param ip
 * @param hw_addr_out
 * @return 0 on success, otherwise failure
 */
static int32_t get_hw_addr(uint32_t ip, uint8_t hw_addr_out[]) {
	uint32_t hash = ip % ARP_CACHE_SIZE;
	for(int i = 0; i < 5; i++) {
		// c_printf("get_hw_addr: ip=0x%08x, hash=%d, _arp_cache[hash].ip_addr=0x%08x\n", ip, hash, _arp_cache[hash].ip_addr);
		for(int index = hash, count = 0; count < ARP_CACHE_SIZE; index++, count++) {
			if(index == ARP_CACHE_SIZE) index = 0; // wrap around
			// c_printf("_arp_cache[%d].ip_addr=0x%08x\n", index, _arp_cache[index].ip_addr);
			if(_arp_cache[index].ip_addr == ip && _arp_cache[index].filled) {
				memcpy(hw_addr_out, _arp_cache[index].hw_addr, 6);
				return 0;
			}
		}
		send_arp_request(ip);
		sleep(1000);
	}
	return -1;
}

int32_t send_ipv4(uint32_t dst_ip, void* data, uint32_t length, ip_protocol_t protocol) {
	// Get HW addr
	uint8_t hw_addr[6];
	if(get_hw_addr(dst_ip, hw_addr)) {
		c_printf("NIC: Couldn't find specified IP\n");
		return -1;
	}
	// c_printf("NIC: send_ipv4 found hw_addr: ");
	print_mac_addr(hw_addr);
	c_printf("\n");

	ipv4_t ipv4;

	ipv4.version = 4;
	ipv4.ihl = 5;
	ipv4.dscp = 0;
	ipv4.ecn = 0;
	// ipv4.total_len = length + IPV4_HEAD_LEN;
	uint16_t ip_length = length + IPV4_HEAD_LEN;
	// c_printf("length (le): 0x%04x, length (be): 0x%04x\n", length + IPV4_HEAD_LEN, be_length);
	ipv4.total_len = __builtin_bswap16(ip_length);
	ipv4.id = 0;
	ipv4.flags = 0;
	ipv4.frag_offset = 0;
	ipv4.ttl = 64;
	ipv4.protocol = protocol;
	ipv4.header_checksum = 0; // only for now
	ipv4.src_ip = __builtin_bswap32(_nic.my_ip);
	ipv4.dst_ip = __builtin_bswap32(dst_ip);
	ipv4.header_checksum = ip_chksm(&ipv4, IPV4_HEAD_LEN);

	memcpy(ipv4.ip_data, data, length);
	return send_packet(hw_addr, &ipv4, ip_length, ethertype_ipv4);

	// memcpy(cb->u.tcb.eth_packet.dst_mac, dst_hw_addr, MAC_LENGTH_BYTES);
	// cb->u.tcb.eth_packet.payload.ipv4.version = 4;
	// cb->u.tcb.eth_packet.payload.ipv4.ihl = 5;
	// cb->u.tcb.eth_packet.payload.ipv4.dscp = 
	// cb->u.tcb.eth_packet.payload.ipv4.ecn = 0;
	// cb->u.tcb.eth_packet.payload.ipv4.total_len = length + IPV4_HEAD_LEN;
	// cb->u.tcb.eth_packet.payload.ipv4.id = 0;
	// cb->u.tcb.eth_packet.payload.ipv4.flags = 0;
	// cb->u.tcb.eth_packet.payload.ipv4.frag_offset = 0;
	// cb->u.tcb.eth_packet.payload.ipv4.ttl = 64;
	// cb->u.tcb.eth_packet.payload.ipv4.protocol = ip_udp;
	// cb->u.tcb.eth_packet.payload.ipv4.header_checksum = 0; // only for now
	// memcpy(cb->u.tcb.eth_packet.payload.ipv4.src_ip, _nic.my_ip, 4);
	// memcpy(cb->u.tcb.eth_packet.payload.ipv4.dst_ip, dst_ip, 4);
	// cb->u.tcb.eth_packet.payload.ipv4.header_checksum = ip_chksm(&(cb->u.tcb.eth_packet.payload.ipv4), IPV4_HEAD_LEN);

	// memcpy(cb->u.tcb.eth_packet.payload.ipv4.ip_data, data, length);

	// uint8_t status = mem_read8(&_nic.csr->scb.status);
	// if(((status & cu_mask) == cu_idle) 
	// 		|| ((status & cu_mask) == cu_suspended)) {
	// 	mem_write32(&_nic.csr->scb.gen_ptr, (uint32_t) cb);
	// 	mem_write8(&_nic.csr->scb.command, cuc_start);
	// 	write_flush(&_nic);
	// }
	// return 0;
}

void set_ip(uint32_t ip) {
	_nic.my_ip = ip;
}

void hexdump(void* data, uint32_t length, uint32_t bytes_per_line) {
	uint8_t* data_ = (uint8_t*) data;
	c_printf("hexdump(data=0x%08x, length=%d, bytes_per_line=%d):\n", (uint32_t) data, length, bytes_per_line);
	for(uint32_t i = 0; i < length; i+=bytes_per_line) {
		c_printf("[%5x]", i);
		for(uint32_t n = 0; n < bytes_per_line; n++) {
			if(n % 2 == 0) {
				c_printf(" ");
			}
			if(n + i >= length) {
				c_printf("  ");
			}
			else {
				c_printf("%02x", data_[n + i]);
			}
		}
		c_printf("  |");
		for(uint32_t n = 0; n < bytes_per_line; n++) {
			if(n % 8 == 0) {
				c_printf(" ");
			}
			if(n + i >= length) {
				c_printf(" ");
			}
			else {
				c_printf("%c", ascii_to_printable_char(data_[n + i]));
			}
		}
		c_printf(" |\n");
	}
}

int32_t nic_tx_daemon(void* arg) {
	for(;;) {
		// check_waiting_transmissions();
		recycle_command_blocks();
		sleep(500);
	}
}

int32_t nic_rx_daemon(void* arg) {
	(void) arg;
	intel_nic_enable_rx();
	for(;;) {
		// sleep(200);
		// c_printf("rfa.head=0x%08x, rfa.next=0x%08x, rfa.tail=0x%08x\n", _nic.rfa.head, _nic.rfa.next, _nic.rfa.tail);
		while(_nic.rfa.head != _nic.rfa.next) {
			rfd_t* rfd = _nic.rfa.head;
			uint16_t byte_count = rfd->count & 0x3FFF;
			eth_packet_rx_t* packet = (eth_packet_rx_t*) rfd->data;
			hexdump(rfd->data, byte_count, 16);
			// hexdump(rfd->data, byte_count, 32);
			
			if(packet->ethertype == ethertype_arp) {
				// c_printf("hw_type: %x\n", packet->payload.arp.hw_type);
				// c_printf("protocol_type: %x\n", packet->payload.arp.protocol_type);
				// c_printf("hw_addr_len: %x\n", packet->payload.arp.hw_addr_len);
				// c_printf("protocol_addr_len: %x\n", packet->payload.arp.protocol_addr_len);
				// c_printf("opcode: %x\n", packet->payload.arp.opcode);
				// c_printf("sender_hw_addr: %x\n", packet->payload.arp.sender_hw_addr);
				// c_printf("sender_protocol_addr: %x\n", packet->payload.arp.sender_protocol_addr);
				// c_printf("target_hw_addr: %x\n", packet->payload.arp.target_hw_addr);
				// c_printf("target_protocol_addr: %x\n", packet->payload.arp.target_protocol_addr);
				if(packet->payload.arp.opcode == arp_request) {
					c_printf("NIC: received an ARP request\n");
					send_arp_reply(packet->payload.arp.sender_protocol_addr, packet->payload.arp.sender_hw_addr);
					save_hw_addr(__builtin_bswap32(packet->payload.arp.sender_protocol_addr), packet->payload.arp.sender_hw_addr);
				}
				else if(packet->payload.arp.opcode == arp_reply) {
					c_printf("NIC: received an ARP reply\n");
					save_hw_addr(__builtin_bswap32(packet->payload.arp.sender_protocol_addr), packet->payload.arp.sender_hw_addr);
				}
			}
			else if(packet->ethertype == ethertype_ipv4) {
				switch(packet->payload.ipv4.protocol) {
					case ip_icmp:
					{
						icmp_t* icmp = (icmp_t*) &packet->payload.ipv4.ip_data;
						c_printf("NIC received ICMP - type=%d, code=%d\n", icmp->type, icmp->code);
						// icmp.type
						// icmp.code
						// icmp.chksm
						// icmp.rest_of_header
						
						// do icmp things
							
					}
						break;

					case ip_igmp:
						// do igmp things
						break;

					case ip_tcp:
						// don't do tcp things
						break;

					case ip_udp:
						// maybe do udp things
						break;

					default:
						break;
				}
			}
			else { // if(packet->ethertype < 1500) {

			}
			_nic.rfa.head = (rfd_t*) rfd->link;
			free_page(rfd);
			_nic.rfa.tail->link = (uint32_t) make_new_tail();
			_nic.rfa.tail->command &= ~(0x8000); // unset EL
			_nic.rfa.tail = (rfd_t*) _nic.rfa.tail->link;
		} // while(_nic.rfa.head != _nic.rfa.next)
	} // for(;;)
	return 0; // keep the compiler happy
}

void intel_nic_init() {
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
	// uint8_t interrupt_pin = pci_cfg_read_byte(bus, slot, 0, PCI_INTERRUPT_PIN); // showing up as 0x01
	// uint8_t interrupt_line = pci_cfg_read_byte(bus, slot, 0, PCI_INTERRUPT_LINE); // showing up as 0x0B
	// c_printf("NIC: interrupt pin = 0x%02x\ninterrupt line = 0x%02x\n", interrupt_pin, interrupt_line);
	__install_isr(NET_INTEL_INT_VECTOR, intel_nic_handler);

	// 
	// Setup the CBL, RFA, rx buffer, and ARP cache
	// 
	c_printf("NIC: Initializing CBL\n");
	init_cbl(&_nic, TOTAL_CB);
	c_printf("NIC: Initializing RFA\n");
	init_rfa(&_nic, TOTAL_RFD);
	c_printf("NIC: Initializing ARP cache\n");
	_arp_cache = (arp_entry_t*) get_next_page();
	memset(_arp_cache, 0, PAGE_SIZE);
	//
	// Initial Configure command
	// 
	c_printf("NIC: Sending initial configure command\n");
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
	// Set a default IP address
	//
	// uint8_t ip[4] = {0xA9, 0xFE, 0xA9, 0xB9};
	// set_ip(ip);
	set_ip(0xA9FEA9B9);

	// 
	// 
	// TODO:
	// o issue ARP requests
	// o handle ARP reply/request and add to cache on both
	// o ICMP ping
	// o ARP cache
	// o add syscalls
	// o write send_ipv4()
	// o keep flag for rx_enable in _nic. with first call to receive, enable rx
	// o mutex on doing anything with CB
	// 
	// TEST:
	// 
	// DONE:
	// o add rx_daemon
	// o receive ARP request, and reply to it
	// o gratuitous ARP
	// o finish send_arp(), and handle arp packets -- get IPs working so we can make some applications!
	// o convert send_grat_arp() to send_arp(), so that it can handle sending arp requests/gratuitous/replies
	// o merge into master
	// o write hexdump(void* data, uint32_t length) using code from claim_rfd_data
	// o handle receive interrupts in addition to CU interrupts
	// o enable receiving data by putting first RFD ptr into gen_ptr (pg 99)
	// o configure receive buffers
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

void intel_nic_enable_rx() {
	mem_write32(&_nic.csr->scb.gen_ptr, (uint32_t) _nic.rfa.next);
	mem_write8(&_nic.csr->scb.command, ruc_start);
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