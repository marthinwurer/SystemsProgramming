#include <kern/net/net_test.h>
#include <kern/net/intel.h>
#include <kern/pci/pci.h>
#include <baseline/c_io.h>
#include <kern/memory/memory_map.h> //get_next_page, free_page

#include <stdint.h>


int32_t net_test_main(void* args) {
	(void) args; // surpress warnings

	//
	// Do test things here...
	//
	uint8_t* my_data = (uint8_t*) get_next_page();
	for(uint32_t i = 0; i < 0x80; i++) {
		my_data[i] = 0xBA;
	}
	uint8_t dst_mac[6] = {0x00, 0xE0, 0x7C, 0xC8, 0x7D, 0x08};
	for(uint32_t i = 0; i < 3; i++) {
		c_printf("Sending packet #%d\n", i);
		send_packet(dst_mac, my_data, 0x80);
	}


	return 0;
}

