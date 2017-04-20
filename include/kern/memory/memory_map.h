/*
 * memory_map.h
 */

#pragma once

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



/**
 * Does a BIOS interrupt and gets the memory map.
 * returns a pointer to the first item in the map.
 * Size is overwritten with the number of memory map entries.
 */
memory_map_entry * get_memory_map(uint32_t * size);
