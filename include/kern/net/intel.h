#ifndef _KERN_NET_INTEL_H
#define _KERN_NET_INTEL_H

#include <kern/memory/memory_map.h> //PAGE_SIZE
#include <baseline/common.h>

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
 * 
 */

#define NET_INTEL_VENDOR 0x8086
#define NET_INTEL_QEMU_NIC 0x100E
#define NET_INTEL_DSL_NIC 0x1229

#define NET_INTEL_INT_VECTOR 0x2B

#define TOTAL_CB 0x80
#define TOTAL_RFD 0x80

#define MAX_EEPROM_LENGTH 256
#define MAC_LENGTH_BYTES 6

#define NET_INTEL_CFG_LENGTH 22
#define NET_INTEL_TCB_MAX_DATA_LEN 2600
#define NET_INTEL_MIN_ETH_LENGTH 46
#define NET_INTEL_MAX_ETH_LENGTH 1500
#define NET_INTEL_ETH_HEAD_LEN 14
#define NET_INTEL_ARP_HEAD_LEN 28
#define NET_INTEL_RFD_SIZE 3096
#define NET_INTEL_RX_BUF_MAX_LEN 3096

#define ETHERTYPE_IPv4 0x0800
#define ETHERTYPE_ARP 0x0806
#define ETHERTYPE_IPX 0x8137
#define ETHERTYPE_IPv 0x86dd

// Stores info about network interface
struct nic_info {
	struct csr * csr;
	uint8_t mac[MAC_LENGTH_BYTES];
	uint16_t eeprom_count;
	uint16_t eeprom[MAX_EEPROM_LENGTH]; // biggest eeprom will be 256 16-bit words
	uint32_t avail_cb;
	struct cb* next_cb;
	struct cb* cb_to_check;
	struct rfd* next_rfd;
	uint32_t rx_buf_count;
	struct rx_buf* rx_buf_head;
	struct rx_buf* next_rx_buf;
};

// MMIO Control Status Register
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

// Command Block
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
			struct {
				uint8_t dst_mac[MAC_LENGTH_BYTES];
				// uint8_t src_mac[MAC_LENGTH_BYTES];
				uint16_t ethertype; // under 1500 is payload length, above is type of payload header
				union {
					uint8_t data[NET_INTEL_TCB_MAX_DATA_LEN]; 
					struct {
						uint16_t hw_type;
						uint16_t protocol_type;
						uint8_t hw_addr_len;
						uint8_t protocol_addr_len;
						uint16_t opcode;
						uint8_t sender_hw_addr[6];
						uint32_t sender_protocol_addr; // unaligned
						uint8_t target_hw_addr[6];
						uint32_t target_protocol_addr;
					} arp;
				} payload;
			} eth_packet;
		} tcb;
	} u;
};

// Receive Frame Descriptor
struct rfd {
	uint16_t status;
	uint16_t command;
	uint32_t link;
	uint32_t __pad1;
	uint16_t count;
	uint16_t size;
	uint8_t data[NET_INTEL_RFD_SIZE];
};

struct rx_buf {
	struct rx_buf* next;
	uint32_t length;
	uint32_t curr_ptr;
	void* data[NET_INTEL_RX_BUF_MAX_LEN];
};

// Constants to control EEPROM
enum eeprom_lo_control {
	EESK = 0x01, // Serial clock
	EECS = 0x02, // Chip select
	EEDI = 0x04, // Serial data in
	EEDO = 0x08  // Serial data out
};

enum scb_stat_ack {
	ack_cs_tno = 0x80, // CU finished executing CB with interrupt bit set
	ack_fr = 0x40, // finished receiving
	ack_cna = 0x20, // CU left active state
	ack_rnr = 0x10, // RU leaves ready state
	ack_mdi = 0x08, // mdi read/write cycle compelte
	ack_swi = 0x04 // SW interrupt
};

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

enum eeprom_opcodes {
	op_write = 0x05,
	op_read  = 0x06,
	op_ewds  = 0x10, // Erase/write disable
	op_ewen  = 0x13, // Erase/write enable
};

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

enum cb_status {
	cb_c = 0x8000,
	cb_ok = 0x2000,
	cb_u = 0x1000
};

	// union {
	// 	u8 iaaddr[ETH_ALEN];
	// 	__le32 ucode[UCODE_SIZE];
	// 	struct config config;
	// 	struct multi multi;
	// 	struct {
	// 		u32 tbd_array;
	// 		u16 tcb_byte_count;
	// 		u8 threshold;
	// 		u8 tbd_count;
	// 		struct {
	// 			__le32 buf_addr;
	// 			__le16 size;
	// 			u16 eol;
	// 		} tbd;
	// 	} tcb;
	// 	__le32 dump_buffer_addr;
	// } u;
	// struct cb *next, *prev;
	// dma_addr_t dma_addr;
	// struct sk_buff *skb;

int32_t send_packet(uint8_t dst_hw_addr[], void* data, uint16_t length);
void intel_nic_enable_rx();

/**
 * hexdump, only prints in multiples of 8 bytes, or it will overrun the
 * data
 *
 * @param data pointer to data to print
 * @param length bytes of data to print
 */
void hexdump(void* data, uint32_t length);
void intel_nic_init();

#endif

