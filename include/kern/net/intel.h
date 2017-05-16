#ifndef _KERN_NET_INTEL_H
#define _KERN_NET_INTEL_H

#include <kern/memory/memory_map.h> //PAGE_SIZE
#include <baseline/common.h>

/**
 * Intel Network driver. Contains needed structures and methods to perform network functions. 
 * @author Daniel Meiller
 */

/**
 * Intel Shared Memory Architecture
 * 
 * ---- Defintions ----
 * CSR := Control Status Register
 * SCB := System Control Block
 * 
 * CU := Command Unit
 * CB := Command Block
 * CBL := Command Block List
 * TCB := Transmit Command Block
 * TBD := Transmit Buffer Descriptor
 * IA := Individual Address
 * 
 * RU := Receive Unit
 * RFD := Receive Frame Descriptor
 * RFA := Receive Frame Area
 * 
 * ARP := Address Resolution Protocol
 * 
 */

#define NET_INTEL_VENDOR 0x8086
#define NET_INTEL_QEMU_NIC 0x100E
#define NET_INTEL_DSL_NIC 0x1229

#define NET_INTEL_INT_VECTOR 0x2B

#define TOTAL_CB 0x80
#define TOTAL_RFD 1024
#define ARP_CACHE_SIZE 256

#define MAX_EEPROM_LENGTH 256
#define MAC_LENGTH_BYTES 6

#define NET_INTEL_CFG_LENGTH 22
#define NET_INTEL_TCB_MAX_DATA_LEN 2600
#define NET_INTEL_MIN_ETH_LENGTH 46
#define NET_INTEL_MAX_ETH_LENGTH 1500
#define NET_INTEL_ETH_HEAD_LEN 14
#define NET_INTEL_ARP_HEAD_LEN 28
#define IPV4_HEAD_LEN 20
#define NET_INTEL_RFD_SIZE 3096
#define NET_INTEL_RX_BUF_MAX_LEN 3096

/**
 * Receive Frame Descriptor is given to receive unit to store incoming
 * transmissions. 
 */
typedef struct {
	uint16_t status;
	uint16_t command;
	uint32_t link;
	uint32_t __pad1;
	uint16_t count;
	uint16_t size;
	uint8_t data[NET_INTEL_RFD_SIZE];
} rfd_t;

/**
 * Stores info about network interface
 */
struct nic_info {
	struct csr * csr;
	uint8_t mac[MAC_LENGTH_BYTES];
	uint16_t eeprom_count;
	uint16_t eeprom[MAX_EEPROM_LENGTH]; // biggest eeprom will be 256 16-bit words
	uint32_t avail_cb;
	struct cb* next_cb;
	struct cb* cb_to_check;
	struct {
		rfd_t* tail;
		rfd_t* head;
		rfd_t* next;
	} rfa;
	// uint8_t my_ip[4];
	uint32_t my_ip;
};

/**
 * MMIO Control Status Register
 */
struct csr {
	struct {
		uint8_t status;
		uint8_t stat_ack;
		uint8_t command;
		uint8_t interrupt_mask;
		uint32_t gen_ptr;
	} scb;
	uint32_t port;
	uint16_t __pad1;
	uint8_t eeprom_lo;
	uint8_t eeprom_hi;
	uint32_t mdi;
	uint32_t rx_dma_byte_count;
};

typedef struct {
	uint8_t ihl : 4;
	uint8_t version : 4;
	uint8_t dscp : 6;
	uint8_t ecn : 2;
	uint16_t total_len;
	uint16_t id;
	uint16_t flags : 3;
	uint16_t frag_offset : 13;
	uint8_t ttl;
	uint8_t protocol;
	uint16_t header_checksum;
	uint32_t src_ip;
	uint32_t dst_ip;
	// uint8_t src_ip[4];
	// uint8_t dst_ip[4];
	uint8_t ip_data[NET_INTEL_TCB_MAX_DATA_LEN - 5];
} ipv4_t;

typedef struct {
	uint16_t hw_type;
	uint16_t protocol_type;
	uint8_t hw_addr_len;
	uint8_t protocol_addr_len;
	uint16_t opcode;
	uint8_t sender_hw_addr[6];
	uint32_t sender_protocol_addr; // unaligned
	// uint8_t sender_protocol_addr[4]; // aligned
	uint8_t target_hw_addr[6];
	// uint8_t target_protocol_addr[4];
	uint32_t target_protocol_addr;
} __attribute__((packed)) arp_t;

/**
 * Ethernet frame data payload
 */
typedef union {
	uint8_t data[NET_INTEL_TCB_MAX_DATA_LEN]; 
	ipv4_t ipv4;
	arp_t arp;
} eth_payload_t;

/**
 * Ethernet TX packet structure
 */
typedef struct {
	uint8_t dst_mac[MAC_LENGTH_BYTES];
	// uint8_t src_mac[MAC_LENGTH_BYTES]; // auto inserted
	uint16_t ethertype; // under 1500 is payload length, above is type of payload header
	eth_payload_t payload;
} eth_packet_t;

/**
 * Ethernet RX packet structure
 */
typedef struct {
	uint8_t dst_mac[MAC_LENGTH_BYTES];
	uint8_t src_mac[MAC_LENGTH_BYTES];
	uint16_t ethertype; // under 1500 is payload length, above is type of payload header
	eth_payload_t payload;
} __attribute__((packed)) eth_packet_rx_t;

/**
 * Command Block
 */
struct cb {
	uint16_t status;
	uint16_t command;
	uint32_t link;
	union {
		uint8_t cfg[NET_INTEL_CFG_LENGTH];
		uint8_t mac_addr[MAC_LENGTH_BYTES];
		struct {
			uint32_t tbd_array;
			uint16_t tcb_byte_count;
			uint8_t threshold;
			uint8_t tbd_count;
			eth_packet_t eth_packet;
		} tcb;
	} u;
};

/**
 * ICMP packet structure
 */
typedef struct {
	uint8_t type;
	uint8_t code;
	uint16_t chksm;
	uint32_t rest_of_header;
} icmp_t;

/**
 * entry in the arp cache
 */
typedef struct {
	uint32_t ip_addr;
	uint16_t filled;
	uint8_t hw_addr[6];
} arp_entry_t;

/**
 * Buffer to store data in once received
 */
// struct rx_buf {
// 	struct rx_buf* next;
// 	uint32_t length;
// 	uint32_t curr_ptr;
// 	void* data[NET_INTEL_RX_BUF_MAX_LEN];
// };

typedef enum {
	ip_icmp = 0x01,
	ip_igmp = 0x02,
	ip_tcp = 0x06,
	ip_udp = 0x11
} ip_protocol_t;

/**
 * ARP opcodes
 */
enum arp_opcode {
	arp_request = 0x0100,
	arp_reply = 0x0200
};

/**
 * Ethertype field for ethernet frame. These are all in the correct network
 * byte order, and do not need to be changed before putting into a frame.
 */
typedef enum {
	ethertype_ipv4 = 0x0008,
	ethertype_arp = 0x0608,
	ethertype_ipx = 0x3781,
	ethertype_ipv6 = 0xdd86
} ethertype_t;

/**
 * Constants to control EEPROM
 */
enum eeprom_lo_control {
	EESK = 0x01, // Serial clock
	EECS = 0x02, // Chip select
	EEDI = 0x04, // Serial data in
	EEDO = 0x08  // Serial data out
};

/**
 * Status/ACK flags are set when an interrupt comes in.
 */
enum scb_stat_ack {
	ack_cs_tno = 0x80, // CU finished executing CB with interrupt bit set
	ack_fr = 0x40, // finished receiving
	ack_cna = 0x20, // CU left active state
	ack_rnr = 0x10, // RU leaves ready state
	ack_mdi = 0x08, // mdi read/write cycle compelte
	ack_swi = 0x04 // SW interrupt
};

/**
 * Status flags for the System Control Block
 */
enum scb_status {
	cu_mask = 0xC0,
	ru_mask = 0x3C,
	cu_idle = 0x00,
	cu_suspended = 0x40,
	cu_lpq_active = 0x80,
	cu_hqp_active = 0xC0,
	ru_idle = 0x00,
	ru_suspended = 0x04,
	ru_no_resources = 0x08,
	ru_ready = 0x10
};


/**
 * Control opcodes for the System Control Block
 */
enum scb_control {
	cuc_nop = 0x00,
	cuc_start = 0x10,
	cuc_resume = 0x20,
	cuc_load_dump_cnt_addr = 0x40,
	cuc_dump_stat_cnt = 0x50,
	cuc_load_cu_base = 0x60,
	cuc_dump_reset_stat_cnt = 0x70,
	cuc_cu_static_resume = 0xA0,
	ruc_nop = 0x00,
	ruc_start = 0x01,
	ruc_resume = 0x02,
	ruc_recv_dma_redir = 0x03,
	ruc_abort = 0x04,
	ruc_load_header_data_size = 0x05,
	ruc_load_ru_base = 0x06
};

/**
 * EEPROM opcodes
 */
enum eeprom_opcodes {
	op_write = 0x05,
	op_read  = 0x06,
	op_ewds  = 0x10, // Erase/write disable
	op_ewen  = 0x13, // Erase/write enable
};

/**
 * Command block commands
 */
enum cb_commands {
	// common
	cb_el = 0x8000, // end list (end of CBL, stop executing CBs)
	cb_s = 0x4000, // suspend
	cb_i = 0x2000, // generate interrupt when CB finishes

	// configure
	cb_cfg_cmd = 0x0002,

	// IA configuration
	cb_ia_cmd = 0x0001, // individual address setup

	// transmit
	cb_sf = 0x0008, // simplified mode - all transmit data in TCB, TBD addr field must equal all 1's
	cb_tx_cmd = 0x0004 // transmit
};

/**
 * Command block status codes
 */
enum cb_status {
	cb_c = 0x8000,
	cb_ok = 0x2000,
	cb_u = 0x1000
};

/**
 * Sends an ethernet frame.
 * 
 * @param dst_hw_addr destination hardware address
 * @param data data to send
 * @param length length of data to send
 * @return 0 on success, otherwise failure
 */
int32_t send_packet(uint8_t dst_hw_addr[], void* data, uint16_t length, ethertype_t ethertype);

/**
 * Sends an ipv4 packet
 *
 * @param dst_ip destination IP address
 * @param data data to send
 * @param length length of data to send
 * @return 0 on success, otherwise failure
 */
int32_t send_ipv4(uint32_t dst_ip, void* data, uint32_t length, ip_protocol_t protocol);

/**
 * Sets the IP address of the network card
 *
 * @param ip new ip address
 */
void set_ip(uint32_t ip);

/**
 * Starts the receive unit
 */
void intel_nic_enable_rx();

/**
 * Prints out data in a readable format. This will round up the number of
 * bytes processed to the nearest 8
 *
 * @param data pointer to data to print
 * @param length number of bytes to read from data pointer
 */
void hexdump(void* data, uint32_t length, uint32_t bytes_per_line);

/**
 * transmit daemon for network, does cleanup
 *
 * @param arg unused
 * @return unused
 */
int32_t nic_tx_daemon(void* arg);

/**
 * receive daemon processes incoming data
 *
 * @param arg unused
 * @return unused
 */
int32_t nic_rx_daemon(void* arg);

/**
 * Initializes the network card on startup
 */
void intel_nic_init();

#endif

