/*
 * memory_map_setup.h
 *
 *  Created on: Apr 20, 2017
 *      Author: benjamin
 */

#ifndef INCLUDE_KERN_EARLY_MEMORY_MAP_SETUP_H_
#define INCLUDE_KERN_EARLY_MEMORY_MAP_SETUP_H_

/**
 * Does a BIOS interrupt and gets the memory map.
 * returns a pointer to the first item in the map.
 * Size is overwritten with the number of memory map entries.
 */
void get_memory_map(void);


#endif /* INCLUDE_KERN_EARLY_MEMORY_MAP_SETUP_H_ */
