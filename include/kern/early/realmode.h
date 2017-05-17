/*
** File: include/early/realmode.h
**
** Author: Brennan Ringey (bjr1251)
**
** Contains the function prototype for real mode utility, int32
** Allows for calling BIOS functions from protected mode
*/

#ifndef _KERN_EARLY_REALMODE_H
#define _KERN_EARLY_REALMODE_H

#include <stdint.h>

struct regs16_s {
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
	uint16_t gs;
	uint16_t fs;
	uint16_t es;
	uint16_t ds;
	uint32_t eflags;
} __attribute__((packed));

typedef struct regs16_s regs16_t;

void __int(short vector, regs16_t *regs);

//
// init function for the int32 function. Must be called before int32 and
// only needs to be called once. This function copies the real mode program
// into the bootloader (address 0x7C00).
//
void init_int32(void);

void int32(int vector, regs16_t *regs);


#endif