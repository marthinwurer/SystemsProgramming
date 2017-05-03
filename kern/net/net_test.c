#include <kern/net/net_test.h>
#include <kern/net/intel.h>
#include <kern/pci/pci.h>
#include <baseline/c_io.h>
#include <kern/memory/memory_map.h> //get_next_page, free_page

#include <baseline/ulib.h> //sleep
#include <stdint.h>


int32_t net_test_main(void* args) {
	(void) args; // surpress warnings

	// c_printf("enabling NIC rx in 5 sec...");
	// sleep(5000);
	intel_nic_enable_rx();
	// c_printf(" enabled, waiting to tx for another 5 sec...\n");
	// sleep(5000);

	//
	// Do test things here...
	//
	uint8_t* my_data = (uint8_t*) get_next_page();
	for(uint32_t i = 0; i < 0x80; i++) {
		my_data[i] = 0xBA;
	}

	uint8_t dst_mac[6] = {0x00, 0xE0, 0x7C, 0xC8, 0x7D, 0x08};

	// char* dummy_data = "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.";
	// send_packet(dst_mac, dummy_data, strlen(dummy_data) + 1);
	uint32_t fast_packets = 0;
	uint32_t slow_packets = 100;
	uint32_t fast_interval_ms = 100;
	uint32_t slow_interval_ms = 2000;
	 
	uint32_t i;
	for(i = 0; i < fast_packets; i++) { // test ring CBL
		c_printf("Sending packet #%d\n", i);
		send_packet(dst_mac, my_data, 0x80);
		sleep(fast_interval_ms);
	}

	for(i = 0; i < slow_packets; i++) { // test ring CBL
		c_printf("Sending packet #%d\n", i + fast_packets);
		send_packet(dst_mac, my_data, 0x80);
		sleep(slow_interval_ms);
	}

	return 0;
}

