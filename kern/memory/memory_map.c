
#include <kern/memory/memory_map.h>
#include <baseline/c_io.h>


void disp_memory_map(void){

	memory_map_entry * map = (memory_map_entry *) MY_MM_ADDRESS;

	// get the memory map size
	uint32_t size = map[0].base_l;

	c_printf("%d\n", size);

	for( uint32_t ii = 1; ii < size; ++ii){
		c_printf("start: %x length: %x type: %d\n", map[ii].base_l, map[ii].length_l, map[ii].type);
	}

	c_printf("%d\n", size);

}
