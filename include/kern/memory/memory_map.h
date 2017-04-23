/*
 * memory_map.h
 */

#ifndef _KERN_MEMORY_MAP_H
#define _KERN_MEMORY_MAP_H

#include <stdint.h>

#define MY_MM_ADDRESS 0x3800

struct memory_map_s{
	uint32_t base_l;
	uint32_t base_h;
	uint32_t length_l;
	uint32_t length_h;
	uint32_t type;
	uint32_t extended;
}__attribute__((packed));


typedef struct memory_map_s memory_map_entry;

void disp_memory_map(void);

#endif
