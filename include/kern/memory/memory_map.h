/*
 * memory_map.h
 */

#ifndef _KERN_MEMORY_MAP_H
#define _KERN_MEMORY_MAP_H

#include <stdint.h>

#define MY_MM_ADDRESS 0x3400

#define MB 			0x100000
#define KB 			0x400
#define PAGE_SIZE	(4 * KB)
#define MAX_MEM		0xFFFFFFFF
#define ALIGN_MASK	0x00000FFF

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




#endif
