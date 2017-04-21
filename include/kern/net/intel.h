#ifndef _KERN_NET_INTEL_H
#define _KERN_NET_INTEL_H

#include <baseline/common.h>

/**
 * Intel Shared Memory Architecture
 * 
 * ---- Defintions ----
 * CSR := Control Status Register
 * SCB := System Control Block
 * CBL := Command Block List
 * RFA := Receive Frame Area
 * CU := Command Unit
 * RU := Receive Unit
 * 
 * 
 * ---- Steps ----
 * 1. Access CSR address (memory mapped or port) using the pci functions.
 * 2. SCB is the first 8 bytes of the CSR.
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 */

// CSR offsets
#define NET_INTEL_SCB_STATUS 0x00
#define NET_INTEL_SCB_COMMAND_BYTE 0x02
#define NET_INTEL_SCB_GENERAL_PTR 0x04
#define NET_INTEL_PORT 0x08
#define NET_INTEL_EEPROM_CTRL_REG 0x0E
#define NET_INTEL_MDI_CTRL_REG 0x10
#define NET_INTEL_RX_DMA_BYTE_COUNT 0x14
#define NET_INTEL_FLOW_CTRL_REG 0x19
#define NET_INTEL_PMDR 0x1B
#define NET_INTEL_GENERAL_CTRL 0x1C
#define NET_INTEL_GENERAL_STATUS 0x1D
#define NET_INTEL_FUNC_EVENT_REG 0x30
#define NET_INTEL_FUNC_EVENT_MASK_REG 0x34
#define NET_INTEL_FUNC_PRESENT_STATE_REG 0x38
#define NET_INTEL_FORCE_EVENT_REG 0x3C

// Vendor and device constants
#define NET_INTEL_VENDOR 0x8086
#define NET_INTEL_QEMU_NIC 0x100E
#define NET_INTEL_DSL_NIC 0x1229

uint32_t intel_csr_read32(uint32_t csr_addr, uint32_t offset);
uint16_t intel_csr_read16(uint32_t csr_addr, uint32_t offset);
uint8_t intel_csr_read8(uint32_t csr_addr, uint32_t offset);

void intel_csr_write32(uint32_t csr_addr, uint32_t offset, uint32_t data);
void intel_csr_write16(uint32_t csr_addr, uint32_t offset, uint16_t data);
void intel_csr_write8(uint32_t csr_addr, uint32_t offset, uint8_t data);

#endif

