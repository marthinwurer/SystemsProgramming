
#include <baseline/bootstrap.h>
#include <kern/gdt/GDTEntry.h>

#define CODE_ACCESS (GDT_ACCESS_PR | GDT_ACCESS_PRIVL_0 | GDT_ACCESS | GDT_ACCESS_EX | GDT_ACCESS_RW)
#define DATA_ACCESS (GDT_ACCESS_PR | GDT_ACCESS_PRIVL_0 | GDT_ACCESS | GDT_ACCESS_RW)


void gdt_init(void) {

	GDTEntry *table = (GDTEntry*)GDT_ADDRESS;

	*table++ = GDT_NULL;
	
	// linear segment
	*table++ = (GDTEntry){
		.limit = 0xFFFF,
		.base = 0x0000,
		.base_lo = 0x00,
		.access = DATA_ACCESS,
		.flags = GDT_FLAGS_GR | GDT_FLAGS_SZ | 0xF,
		.base_hi = 0x00
	};

	// code segment
	*table++ = (GDTEntry){
		.limit = 0xFFFF,
		.base = 0x0000,
		.base_lo = 0x00,
		.access = CODE_ACCESS,
		.flags = GDT_FLAGS_GR | GDT_FLAGS_SZ | 0xF,
		.base_hi = 0x00
	};

	// data segment
	*table++ = (GDTEntry){
		.limit = 0xFFFF,
		.base = 0x0000,
		.base_lo = 0x00,
		.access = DATA_ACCESS,
		.flags = GDT_FLAGS_GR | GDT_FLAGS_SZ | 0xF,
		.base_hi = 0x00
	};

	// stack segment
	*table++ = (GDTEntry){
		.limit = 0xFFFF,
		.base = 0x0000,
		.base_lo = 0x00,
		.access = DATA_ACCESS,
		.flags = GDT_FLAGS_GR | GDT_FLAGS_SZ | 0xF,
		.base_hi = 0x00
	};
	

}