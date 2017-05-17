/*
** File: include/kern/gdt/GDTEntry.h
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors:
**
** Contains the GDTEntry typedef
*/

#ifndef _KERN_GDT_GDTENTRY_H
#define _KERN_GDT_GDTENTRY_H

#include <stdint.h>

#define GDT_LIMIT_MASK 0x0F
#define GDT_FLAGS_MASK 0xF0

#define GDT_ACCESS 0x10
#define GDT_ACCESS_PR 0x80
#define GDT_ACCESS_PRIVL_MASK 0x60
#define GDT_ACCESS_PRIVL_0 0x00
#define GDT_ACCESS_PRIVL_1 0x20
#define GDT_ACCESS_PRIVL_2 0x40
#define GDT_ACCESS_PRIVL_3 GDT_ACCESS_PRIVI_MASK
#define GDT_ACCESS_EX 0x08
#define GDT_ACCESS_DC 0x04
#define GDT_ACCESS_RW 0x02
#define GDT_ACCESS_AC 0x01

#define GDT_FLAGS_GR 0x80
#define GDT_FLAGS_SZ 0x40

struct GDTEntry_s {
	uint16_t limit;           // Limit 0:15        bits (0-15)
	uint16_t base;            // Base 0:15         bits (16-31)
	uint8_t base_lo;          // Base 16:23        bits (32-39)
	uint8_t access;           // Access byte 0:7   bits (40-47)
	uint8_t flags;            // limit 16:19       bits (48-51)
	                          // flags 0:3         bits (52-55)
	uint8_t base_hi;          // Base 24:31        bits (56-63)
} __attribute__((__packed__));

typedef struct GDTEntry_s GDTEntry;

#define GDT_NULL (GDTEntry){ \
	.limit = 0, \
	.base = 0, \
	.base_lo = 0, \
	.access = 0, \
	.flags = 0, \
	.base_hi = 0 \
}

#endif