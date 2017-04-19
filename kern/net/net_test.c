#include <kern/net/net_test.h>
#include <kern/pci/pci.h>

int32_t net_test_main(void* args) {
	c_printf("in net_test_main()");

	// QEMU ONLY!!!
	int16_t slot_e;
	uint8_t slot;	 
	uint8_t vendor = 0x8086; // Intel
	uint8_t device = 0x100E; // QEMU net card
	// uint8_t device = 0x1229; // DSL net card
	if((slot_e = pci_get_slot(vendor, device)) > 0) {
		slot = (uint8_t) slot_e;
		// TODO: mess with the pci config stuff now that we know what slot the network card is in
	}
}

