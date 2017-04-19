#ifndef _KERN_PCI_PCI_H
#define _KERN_PCI_PCI_H

// PCI Configuration offsets
#define PCI_VENDOR 0x00
#define PCI_DEVICE 0x02
#define PCI_STATUS 0x04
#define PCI_COMMAND 0x06
#define PCI_CLASS_CODE 0x08
#define PCI_REV_ID 0x0B
#define PCI_BIST 0x0C
#define PCI_HEADER_TYPE 0x0D
#define PCI_LATENCY_TIMER 0x0E
#define PCI_CACHE_LINE_SIZE 0x0F
#define PCI_CSR_MEM_MAPPED_BASE_ADDR_REG 0x10
#define PCI_CSR_IO_MAPPED_BASE_ADDR_REG 0x14
#define PCI_FLASH_MEM_MAPPED_BASE_ADDR_REG 0x18
#define PCI_SUBSYSTEM_ID 0x2C
#define PCI_SUBSYSTEM_VENDOR_ID 0x2E
#define PCI_EXPANSION_ROM_BASE_ADDR_REG 0x30
#define PCI_CAP_PTR 0x37
#define PCI_MAX_LATENCY 0x3C
#define PCI_MIN_GRANT 0x3D
#define PCI_INTERRUPT_PIN 0x3E
#define PCI_INTERRUPT_LINE 0x3F
#define PCI_POWER_MANAGEMENT_CAPABILITIES 0xDC
#define PCI_NEXT_ITEM_PTR 0xDE
#define PCI_CAPABILITY_ID 0xDF
#define PCI_DATA 0xE1
#define PCI_POWER_MANAGEMENT_CSR 0xE2

// TODO: macros for the length of each of these

/**
 * pci_cfg_read
 * 
 * http://wiki.osdev.org/PCI#Configuration_Space
 * 
 */
uint16_t pci_cfg_read(uint8_t bus, uint8_t slot,
							uint8_t func, uint8_t offset);

/**
 * pci_get_vendor
 * 
 * http://wiki.osdev.org/PCI#Configuration_Space
 * 
 */
uint16_t pci_get_vendor(uint8_t bus, uint8_t slot);

/**
 * pci_get_slot
 * 
 * Gets the pci slot for a specified vendor and device
 * @return -1 on failure, otherwise pci slot number
 * 
 */
int16_t pci_get_slot(uint16_t vendor, uint16_t device);

/**
 * pci_enumerate
 * 
 * Prints out devices on the PCI bus
 */
void pci_enumerate(uint8_t bus);

#endif