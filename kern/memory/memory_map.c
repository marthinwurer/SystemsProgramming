
#include <kern/memory/memory_map.h>

// to integer and to pointer
#define TI			(uint32_t)
#define TP			(uint32_t *)

uintptr_t max_address = 0;

uintptr_t pat_size = 0x20000;

uint8_t * page_availibility_table = NULL;
size_t last_availible = 0;

memory_map_entry * map = (memory_map_entry *) MY_MM_ADDRESS;

uint32_t mm_size = 0;

address_space_t page_directory = 0;
uint32_t * page_table;


static void __page_fault_isr( int vector, int code ){
	__panic("Page Fault received!");
	(void) vector;
	(void) code;
}




void disp_memory_map(void){
	// for reference: http://www.uruk.org/orig-grub/mem64mb.html


	// get the memory map size
	mm_size = map[0].base_l;

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

	// pages are 4KB
	c_printf("%x\n", (((PAGE_SIZE * 8) >> 15) & 0xFFFFF));





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
		c_printf("%x\n", offset );
	}else{
		__panic("Couldn't mark upper memory as used!");
	}

	// mark the space that the table occupies as used.
	for(uintptr_t offset = 0; offset < pat_size; offset += PAGE_SIZE ){
		set_pat(((uintptr_t) page_availibility_table) + offset, 0xFF);
	}

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
//			c_printf("next address: %x\n", address );

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



/**
 * Generates a page table that maps 1:1 with memory and enables paging with it installed.
 */
void setup_initial_page_table(void){

	// get a page for the page_directory
	page_directory = get_next_page();


	// make page tables for each directory
	for( int ii = 0; ii < KB; ++ii){
		uint32_t * cur_pt = get_next_page();
		page_directory[ii] = (uint32_t) cur_pt | PAGE_FLAGS;
		for( int jj = 0; jj < KB; ++jj){
			cur_pt[jj] = (ii << 22) | ( jj << 12 ) | PAGE_FLAGS;

		}
		c_printf("pte:%x\n", cur_pt[0]);
	}

	// store the identity mapped first entry
	page_table = (uint32_t *) page_directory[0];

	// install the page fault ISR
	__install_isr( INT_VEC_PAGE_FAULT, __page_fault_isr );


	set_page_directory(page_directory);
}

/**
 * sets the current page directory and enables paging and memory protection.
 */
address_space_t set_page_directory(address_space_t directory){

	c_printf("setting pd to %x\n", directory);

	address_space_t old = NULL;


	// remember, AT&T syntax
	// set the page directory
	__asm__
	__volatile__      /* optional */
	(
		"movl %%cr3, %%ebx\n"
		"movl %%eax, %%cr3"
			:"=b"(old) /*output operands optional */
			  : "a"(directory) /*input operands  optional */
				: //clobber list    /* optional */
	);

	c_printf("old pd: %x\n",old);
//	__panic("done setting pd");
	c_printf("enabling paging\n");

	// enable paging
	__asm__
	(
		"movl %%cr0, %%eax\n"
		"orl $0x80000001, %%eax\n"
		"movl %%eax, %%cr0"
		: : : "eax"
	);

	c_printf("paging done\n");
//	__panic("done with paging setup");



	return old;
}

/**
 * disables paging and returns the previous page directory.
 */
uint32_t * disable_paging(void){
	return NULL;
}

uint32_t * new_page_table(void){
	uint32_t * table = get_next_page();
	memset(table, 0, PAGE_SIZE);
	return table;
}

/**
 * Map a section of physical memory to virtual memory. If pmem_start is null,
 * maps free pages from pmemstart on to fill out length.
 */
void * mmap(uint32_t * vspace,
		void * vmem_start,
		void * pmem_start,
		size_t length,
		int * status){

	// if we're remapping the identity paging, then explode forcefully.
	// this includes the lower index.
	if( vspace == page_directory || (TI vmem_start) < SECOND_PDE){
		__panic("Cannot remap identity paging");
	}

	// make sure that the pointers are valid offsets
	if( ((uintptr_t)vmem_start | (uintptr_t)pmem_start) & ALIGN_MASK){
		if( status != NULL){
			*status = E_FAILURE;
		}
		return NULL;
	}

	// do the plain mapping.
	if( pmem_start != NULL){
		uintptr_t p_location = (uintptr_t)pmem_start;
		uintptr_t v_location = (uintptr_t)vmem_start;
		uintptr_t done = p_location + length;


		for(; p_location < done; p_location += PAGE_SIZE, v_location += PAGE_SIZE){
			uint32_t pde_index = (v_location >> 22) & KB_MASK;
			uint32_t pte_index = (v_location >> 12) & KB_MASK;

			// if the page directory is null at the index, then we need to allocate
			// a new page table.
			uint32_t * d_entry = (uint32_t*) (vspace[pde_index] & ~ALIGN_MASK);
			if( d_entry == NULL){
				d_entry = new_page_table();
				vspace[pde_index] = (uint32_t)d_entry | PAGE_FLAGS;
				c_printf("New table allocated: %x\n", vspace[pde_index]);
			}

			// add that spot to the page table
			d_entry[pte_index] = p_location | PAGE_FLAGS;
		}

	}else{
		uintptr_t v_location = (uintptr_t)vmem_start;
		uintptr_t done = v_location + length;


		for(; v_location < done;  v_location += PAGE_SIZE){
			uint32_t pde_index = (v_location >> 22) & KB_MASK;
			uint32_t pte_index = (v_location >> 12) & KB_MASK;

			// if the page directory is null at the index, then we need to allocate
			// a new page table.
			uint32_t * d_entry = (uint32_t*) (vspace[pde_index] & ~ALIGN_MASK);
			if( d_entry == NULL){
				d_entry = new_page_table();
				vspace[pde_index] = (uint32_t)d_entry | PAGE_FLAGS;
				c_printf("New table allocated: %x\n", vspace[pde_index]);
			}

			// add that spot to the page table
			d_entry[pte_index] = TI get_next_page() | PAGE_FLAGS;
		}
	}

	if( status != NULL){
		*status = E_SUCCESS;
	}
	return vmem_start;
}

address_space_t new_page_directory(void){
	// get a new page to hold the pde
	address_space_t pde = get_next_page();

	// zero it.
	memset(pde, 0, PAGE_SIZE);

	// identity map the first page
	pde[0] = (uint32_t) page_table;


	return pde;
}

uint32_t * get_phys_address(address_space_t space, uint32_t * virtual_addr){
	uintptr_t v_location = (uintptr_t)virtual_addr;
	uint32_t pde_index = (v_location >> 22) & KB_MASK;
	uint32_t pte_index = (v_location >> 12) & KB_MASK;

	uintptr_t pde_entry = space[pde_index] & ~ALIGN_MASK;
	if( (uint32_t *)pde_entry == NULL){
		return NULL;
	}

	uintptr_t pte_entry = ((uint32_t *) pde_entry)[pte_index] & ~ALIGN_MASK;

	return (uint32_t *)pte_entry;


}


void test_mmap(void){
	uint32_t * second = (uint32_t *) SECOND_PDE;
	address_space_t process_addr = new_page_directory();
	int status = 0;
	mmap(process_addr, second, NULL, PAGE_SIZE, &status);

	c_printf("Physical address: %x\n", get_phys_address(process_addr, second));

	address_space_t identity = set_page_directory(process_addr);
	(void)identity;

	*second = 0xDEADBEEF;

	set_page_directory(identity);

	// get the address from the process address space
	uint32_t * mem = get_phys_address(process_addr, second);
	c_printf("data:%x\n", *mem );

	// remap second to another page
	uint32_t * new_map = get_next_page();
	mmap(process_addr, second, new_map, PAGE_SIZE, &status);

	set_page_directory(process_addr);
	(void)identity;

	*second = 0xDEADBEEF;
	set_page_directory(identity);

	c_printf("data:%x\n", *new_map );

	// should cause a panic
//	mmap(identity, second, new_map, PAGE_SIZE, &status);


//	__panic("lolol");
}
























