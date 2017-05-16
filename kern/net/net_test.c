#include <kern/net/net_test.h>
#include <kern/net/intel.h>
#include <kern/pci/pci.h>
#include <baseline/c_io.h>
#include <kern/memory/memory_map.h> //get_next_page, free_page

#include <baseline/ulib.h> //sleep
#include <stdint.h>


int32_t net_test_main(void* args) {
	(void) args; // suppress warnings

	c_printf("%d:%d\n", 15, __builtin_bswap32(15));
	c_printf("%d:%d\n", 15, __builtin_bswap16(15));

	for(int i = 0; i < 100; i++) {
		send_ipv4(0xA9FEA9B8, "hello, how are you doing today", 31, ip_udp);
		sleep(1000);
	}
	exit(EXIT_SUCCESS);

	// intel_nic_enable_rx();
	// send_grat_arp(0xA9FEA9B9);
	// send_grat_arp(0xC0A88001);

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
		my_data[0] = i;
		send_packet(dst_mac, my_data, 0x80, ethertype_ipv4);
		sleep(fast_interval_ms);
	}

	for(i = 0; i < slow_packets; i++) { // test ring CBL
		my_data[0] = i + fast_packets;
		c_printf("Sending packet #%d\n", i + fast_packets);
		send_packet(dst_mac, my_data, 0x80, ethertype_ipv4);
		sleep(slow_interval_ms);
	}

	exit(EXIT_SUCCESS);
	return 0;
}

