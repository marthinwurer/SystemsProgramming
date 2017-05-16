/*
 * memory_constants.h
 *
 *  Created on: May 16, 2017
 *      Author: benjamin
 */

#ifndef INCLUDE_KERN_MEMORY_MEMORY_CONSTANTS_H_
#define INCLUDE_KERN_MEMORY_MEMORY_CONSTANTS_H_

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


#endif /* INCLUDE_KERN_MEMORY_MEMORY_CONSTANTS_H_ */
