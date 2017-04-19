#ifndef _KERN_PCI_PCI_H
#define _KERN_PCI_PCI_H

// PCI Configuration offsets
#define PCI_VENDOR 0x00
#define PCI_DEVICE 0x02

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