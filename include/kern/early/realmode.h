#ifndef _KERN_EARLY_REALMODE_H
#define _KERN_EARLY_REALMODE_H

#include <stdint.h>

struct regs16_s {
	uint16_t di;
	uint16_t si;
	uint16_t bp;
	uint16_t sp;
	uint16_t bx;
	uint16_t dx;
	uint16_t cx;
	uint16_t ax;
	uint16_t gs;
	uint16_t fs;
	uint16_t es;
	uint16_t ds;
	uint16_t eflags;
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