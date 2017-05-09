
#include <kern/memory/memory_map.h>
#include <baseline/c_io.h>
#include <baseline/klib.h>
#include <baseline/support.h>

#include <stddef.h>


uintptr_t max_address = 0;

uintptr_t pat_size = 0x20000;

uint8_t * page_availibility_table = NULL;
size_t last_availible = 0;

memory_map_entry * map = (memory_map_entry *) MY_MM_ADDRESS;

uint32_t mm_size = 0;


void disp_memory_map(void){
	// for reference: http://www.uruk.org/orig-grub/mem64mb.html


	// get the memory map size
	uint32_t mm_size = map[0].base_l;

	c_printf("%d\n", mm_size);

	for( uint32_t ii = 1; ii <= mm_size; ++ii){
		c_printf("start: %08x%08x length: %08x type: %d 64: %x\n", map[ii].base_h, map[ii].base_l, map[ii].length_l, map[ii].type, map[ii].base);
	}

	c_printf("%d\n", mm_size);


	for( uint32_t ii = 1; ii <= mm_size; ++ii){
		if(map[ii].base_h > 0) continue; // skip higher memory
		uintptr_t current = map[ii].base_l + map[ii].length_l;

		if (current > max_address){
			max_address = current - 1;
		}

	}
	c_printf("max address: %x\n", max_address);


}

void set_pat(uintptr_t address, uint8_t value){
	size_t index = ((address >> 15) & 0x1FFFF);
	uint8_t bit = (address >> 12) & 0x7;
	uint8_t mask = (1 << bit);
	page_availibility_table[index] = (page_availibility_table[index] & ~mask) | (value & mask);
}

uint8_t get_pat(uintptr_t address){
	size_t index = ((address >> 15) & 0x1FFFF);
	uint8_t bit = (address >> 12) & 0x7;
	uint8_t mask = (1 << bit);
	return page_availibility_table[index] & mask;
}

void setup_page_availibility_table(void){

	// find space
	mm_size = map[0].base_l;

	for( uint32_t ii = 1; ii <= mm_size; ++ii){
		if(map[ii].base_h > 0 || map[ii].base_l < 0x100000) continue; // skip higher memory and low memory
		if (map[ii].length_l >= pat_size){
			page_availibility_table = (uint8_t *) map[ii].base_l;
			break;
		}
	}

	// zero the memory
	_kmemset( page_availibility_table, pat_size, 0 );


	// now that the table exists, actually set it up. if there is a 1, it is in use or dangerous.
	for( uint32_t ii = 1; ii <= mm_size; ++ii){
		if(map[ii].base_h > 0 || map[ii].type == FREE_TYPE) continue; // skip higher memory and legal sections

		// loop over the addresses of the the given address range and set them as used
		for(uintptr_t offset = 0; offset < map[ii].length; offset += PAGE_SIZE ){
			set_pat(map[ii].base_l + offset, 0xFF);
		}

	}

	// set the first mb
	for(uintptr_t offset = 0; offset < MB; offset += PAGE_SIZE ){
		set_pat(offset, 0xFF);
	}

	// set from the top memory address to the end
	if (max_address < MAX_MEM - PAGE_SIZE){
		uintptr_t offset;
		for(offset = 0; offset + max_address < MAX_MEM; offset += PAGE_SIZE ){
			set_pat(offset + max_address + 1, 0xFF);
		}
	}else{
		__panic("Couldn't mark upper memory as used!");
	}

	// mark the space that the table occupies as used.
	for(uintptr_t offset = 0; offset < pat_size; offset += PAGE_SIZE ){
		set_pat(((uintptr_t) page_availibility_table) + offset, 0xFF);
	}

	last_availible = 0;

}

/**
 * Get a pointer to the next page in memory that is not being used, and mark it as in use.
 */
void * get_next_page(void){
	// starting at the last index, check if there are free pages
	size_t index = last_availible;
	do{
		uint32_t byte = page_availibility_table[index];
		if((~byte) & 0xFF){
			// it exists in the table

			// find the next free bit
			int next;
			for(next = 0; (byte & (1 << next)); ++next); // we should hit a 0 becasue otherwise the xor would not have triggered.

			// rebuild the address.
			uintptr_t address = index << 15 | next << 12;
			c_printf("next address: %x\n", address );

			// mark it as used
			set_pat(address, 0xFF);

			// set the index again
			last_availible = index;

			return (void *)address;


		}
		index = (index + 1) % pat_size;
	}while( index != last_availible);

	// if there are none, panic

	__panic("No more memory!");
	return NULL;
}


/**
 * Frees a page of memory. if the address is not aligned, then it will fail.
 *
 * prarams:
 * address - the address of the page to free
 *
 * returns:
 * 		0 on success, -1 if the address is not aligned.
 */
int free_page(void * address){

	// check alignment
	if ( (uintptr_t) address & ALIGN_MASK){
		return -1;
	}

	set_pat((uintptr_t) address, 0);
	return 0;
}











