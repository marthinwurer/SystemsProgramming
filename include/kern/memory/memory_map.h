/*
 * memory_map.h
 */

#ifndef _KERN_MEMORY_MAP_H
#define _KERN_MEMORY_MAP_H

#include <x86arch.h>
#include <stdint.h>
#include <baseline/c_io.h>
#include <baseline/klib.h>
#include <baseline/support.h>


#include <stddef.h>
#include <string.h>

#define MY_MM_ADDRESS 0x3800

#define MB 			0x100000
#define KB 			0x400
#define PAGE_SIZE	(4 * KB)
#define SECOND_PDE	(PAGE_SIZE << 10)
#define MAX_MEM		0xFFFFFFFF
#define ALIGN_MASK	0x00000FFF
#define KB_MASK		0x000003FF
#define PAGE_FLAGS	0b000000000011

#define	FREE_TYPE		1
#define	RESERVED_TYPE	2

struct memory_map__32_s{
	uint32_t base_l;
	uint32_t base_h;
	uint32_t length_l;
	uint32_t length_h;
	uint32_t type;
	uint32_t extended;
}__attribute__((packed));

struct memory_map__64_s{
	uint64_t base;
	uint64_t length;
	uint32_t type;
	uint32_t extended;
}__attribute__((packed));

struct mm_entry_s{
	union{
		struct{
			uint64_t base;
			uint64_t length;
		}__attribute__((packed));
		struct{
			uint32_t base_l;
			uint32_t base_h;
			uint32_t length_l;
			uint32_t length_h;
		}__attribute__((packed));
	};
	uint32_t type;
	uint32_t extended;
}__attribute__((packed));


typedef struct mm_entry_s memory_map_entry;

typedef uint32_t * address_space_t;

void disp_memory_map(void);

void setup_page_availibility_table(void);


/**
 * Get a pointer to the next page in memory that is free, and mark it as in use.
 */
void * get_next_page(void);

/**
 * Frees a page of memory. if the address is not aligned, then it will fail.
 *
 * prarams:
 * address - the address of the page to free
 *
 * returns:
 * 		0 on success, -1 if the address is not aligned.
 */
int free_page(void * address);

void setup_initial_page_table(void);

/**
 * Switch to a new address space. given a page directory, changes page directories.
 * returns the old address space
 */
address_space_t set_page_directory(address_space_t directory);

/**
 * Sets the address space that the ISRs will return to.
 */
void set_return_pde(address_space_t dir);

/**
 * Map a section of physical memory to virtual memory. If pmem_start is null,
 * maps free pages from pmemstart on to fill out length.
 * length is in bytes. use the PAGE_SIZE macro and multiply by the number of pages.
 *
 * if you are memory mapping in the kernel, make sure that you make a new address space
 * and switch to and from it to keep the main 1:1 paging intact.
 */
void * mmap(uint32_t * vspace,
		void * vmem_start,
		void * pmem_start,
		size_t length,
		int * status);


/**
 * Creates a new page directory and returns it. The only mapped in it is the
 * identity mapped 1st 1k pages.
 */
address_space_t new_page_directory(void);

uint32_t * get_phys_address(address_space_t space, uint32_t * virtual_addr);

void test_mmap(void);

#endif
