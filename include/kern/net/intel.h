#ifndef _KERN_NET_INTEL_H
#define _KERN_NET_INTEL_H

#include <baseline/common.h>

/**
 * This is SUPER NOT thread safe. 
 * 
 * 
 * 
 * Intel Shared Memory Architecture
 * 
 * ---- Defintions ----
 * CSR := Control Status Register
 * SCB := System Control Block
 * CBL := Command Block List
 * RFA := Receive Frame Area
 * CU := Command Unit
 * RU := Receive Unit
 * TCB := Transmit Command Block
 * TBD := Transmit Buffer Descriptor
 * 
 * 
 */

#define MAC_LENGTH_BYTES 6

// Constants to control EEPROM
enum eeprom_lo_control {
	EESK = 0x01, // Serial clock
	EECS = 0x02, // Chip select
	EEDI = 0x04, // Serial data in
	EEDO = 0x08  // Serial data out
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

// Stores info about network interface
struct nic_info {
	struct csr * csr;
	uint8_t mac[MAC_LENGTH_BYTES];
	uint16_t eeprom_count;
	uint16_t eeprom[256]; // biggest eeprom will be 256 16-bit words
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
	uint16_t reserved1;
	uint8_t eeprom_lo;
	uint8_t eeprom_hi;
	uint32_t mdi;
	uint32_t rx_dma_byte_count;
};

enum cb_commands {
	// common
	cb_el = 0x8000, // end list (end of CBL, stop executing CBs)
	cb_s = 0x4000, // suspend
	cb_i = 0x2000, // generate interrupt when CB finishes

	// IA configuration
	cb_ia_cmd = 0x0001, // individual address setup

	// transmit
	cb_sf = 0x0008, // simplified mode - all transmit data in TCB, TBD addr field must equal all 1's
	cb_tx_cmd = 0x0004 // transmit
};

// Command Block
struct cb {
	uint16_t status;
	uint16_t command;
	uint32_t link;
	union {
		uint8_t mac_addr[MAC_LENGTH_BYTES];
		struct {
			uint32_t tbd_array;
			uint16_t tcb_byte_count;
			uint8_t threshold;
			uint8_t tbd_count;
			struct {
				// uint8_t preamble[7]; // ethernet clock sync preamble
				// uint8_t sfd; // start of frame delimeter
				uint8_t dst_mac[MAC_LENGTH_BYTES];
				uint8_t src_mac[MAC_LENGTH_BYTES];
				uint16_t ethertype; // under 1500 is payload length, above is type of payload header
				uint8_t data[0x40]; // 46-1500 bytes
			} eth_header;
		} tcb;
	} u;
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

// Vendor and device constants
#define NET_INTEL_VENDOR 0x8086
#define NET_INTEL_QEMU_NIC 0x100E
#define NET_INTEL_DSL_NIC 0x1229

void intel_nic_init();

void print_mac_addr(uint8_t mac[]);
// uint32_t detect_eeprom();
// uint16_t eeprom_read(uint8_t addr);
// uint32_t read_mac_addr(uint8_t* mac[]);



uint32_t mem_read32(void* addr);
uint16_t mem_read16(void* addr);
uint8_t mem_read8(void* addr);

void mem_write32(void* addr, uint32_t value);
void mem_write16(void* addr, uint16_t value);
void mem_write8(void* addr, uint8_t value);

#endif

