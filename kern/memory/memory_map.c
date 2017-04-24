
#include <kern/memory/memory_map.h>
#include <baseline/c_io.h>


void disp_memory_map(void){

	memory_map_entry * map = (memory_map_entry *) MY_MM_ADDRESS;

	// get the memory map size
	uint32_t size = map[0].base_l;

	c_printf("%d\n", size);

	for( uint32_t ii = 1; ii <= size; ++ii){
		c_printf("start: %08x%08x length: %08x type: %d 64: %x\n", map[ii].base_h, map[ii].base_l, map[ii].length_l, map[ii].type, map[ii].base);
	}

	c_printf("%d\n", size);

	uintptr_t max_address = 0;

	for( uint32_t ii = 1; ii <= size; ++ii){
		uintptr_t current = map[ii].base_l + map[ii].length_l;

		if (current > max_address){
			max_address = current;
		}

	}
	c_printf("max address: %x\n", max_address);


}
