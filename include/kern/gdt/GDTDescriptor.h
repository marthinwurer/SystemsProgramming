/*
** File: include/kern/gdt/GDTDescriptor.h
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors:
**
** Contains the GDTDescriptor typedef
*/

#ifndef _KERN_GDT_GDTDESCRIPTOR_H
#define _KERN_GDT_GDTDESCRIPTOR_H

#include <stdint.h>

struct GDTDescriptor_s {

	uint16_t size;
	uint32_t offset;

} __attribute__((__packed__));

typedef struct GDTDescriptor_s GDTDescriptor;

#endif