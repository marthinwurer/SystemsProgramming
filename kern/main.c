/*
** SCCS ID:	@(#)main.c	1.3	03/15/05
**
** File:	main.c
**
** Author:	K. Reek
**
** Contributor:	Warren R. Carithers
**
** Description:	Dummy main program
*/

#include <baseline/support.h>
#include <kern/vga13/vga13.h>
#include <kern/vgacolor.h>
#include <kern/realmode.h>
#include <baseline/c_io.h>

#include <kern/vesa/VBEInfo.h>
#include <kern/vesa/vbe.h>
#include <stdint.h>
#include <stddef.h>

//static VBEInfo *info = (VBEInfo*)0x500;



#define physAddr(segment, offset) ((segment * 0x10) + offset)

// void dumpRegs(regs16_t *regs) {
// 	c_printf("di: %x    si: %x     bp: %x    sp: %x\n", regs->di, regs->si, regs->bp, regs->sp);
// 	c_printf("bx: %x\n", regs->bx);
// 	c_printf("dx: %x\n", regs->dx);
// 	c_printf("cx: %x\n", regs->cx);
// 	c_printf("ax: %x\n", regs->ax);
// 	c_printf("gs: %x fs: %x es: %x ds: %x\n", regs->gs, regs->fs, regs->es, regs->ds);
// 	c_printf("eflags: %x\n", regs->eflags);
// }


int main(void) {

	c_clearscreen();

	__init_interrupts();

	__init_int32();


	// regs16_t regs;
	// memset(&regs, 0, sizeof(regs16_t));

	// regs.ax = 0x4F00;
	// regs.di = 0x500;

	// dumpRegs(&regs);

	// c_puts("\nPerforming __int32\n\n");

	// __int32(0x10, &regs);

	// dumpRegs(&regs);


	// info->signature[0] = 'V';
	// info->signature[1] = 'B';
	// info->signature[2] = 'E';
	// info->signature[3] = '2';
	

	// regs16_t regs;
	// regs.es = 0;
	// regs.di = info;
	// regs.ax = 0x4F00;
	// __int32(0x10, &regs);

	VBEInfo info;
	uint16_t vbeResult;
	int result = vbe_getInfo(&info, &vbeResult);

	if (result) {
	// if (regs.ax == 0x4F) {
		c_printf("Failed to get VBEInfo (%x)\n", vbeResult);
	} else {
		c_printf("vbe_getInfo success (%x)\n", vbeResult);
		c_printf("Version: %x\n", info.version);
		c_printf("Video Memory: %d 64KB blocks\n", info.videoMemory);
		c_printf("OEM: %s (0x%x)\n", (const char *)info.oem, info.oem);
		c_printf("Vendor: %s (0x%x)\n", (const char *)info.vendor, info.vendor);
		
		//c_printf("Supported Modes:\n");
		uint16_t segment = info.videoModes >> 16;
		uint16_t offset = info.videoModes & 0xFFFF;
		uint16_t *modes = (uint16_t*)physAddr(segment, offset);

		int modeCount = 0;
		for (uint16_t *curMode = modes; *curMode != 0xFFFF; ++curMode) {
			//c_printf(" [%x] * 0x%x\n", curMode, *curMode);
			modeCount++;
		}

		c_printf("Available Modes: %d\n", modeCount);

		c_puts("Getting info for first available mode...");
		VBEModeInfo modeInfo;
		if (vbe_getModeInfo(*modes, &modeInfo, NULL)) {
			c_puts("Failed\n");
		} else {
			c_puts("Success\n");
			c_printf("Width: %d\n", modeInfo.width);
			c_printf("Height: %d\n", modeInfo.height);
			c_printf("bpp: %d\n", modeInfo.bpp);
			c_printf("Framebuffer: 0x%x\n", modeInfo.framebuffer);

			//vbe_setMode(*modes, NULL);
		}

		// while (*modes != 0xFFFF) {
		// 	c_printf(" * 0x%x\n", *modes);
		// 	++modes;
		// }
	}


	// //set VGA mode 0x13 using BIOS function
	// regs16_t regs;
	// regs.ax = 0x13;
	// __int32(0x10, &regs);

	// c_puts("WHORES");

	// vga13_init();

	// uint8_t rowBuf[VGA13_WIDTH];
	// uint8_t color = 0;
	// for (int i = 0; i != VGA13_WIDTH; ++i) {
	// 	rowBuf[i] = color++;
	// }

	// for (int y = 0; y != VGA13_HEIGHT; ++y) {
	// 	vga13_setRow(y, rowBuf);
	// }

	// regs.ax = 0;
	// __int32(0x16, &regs);

	// regs.ax = 3;
	// __int32(0x10, &regs);


// #define RECT_WIDTH 128
// #define RECT_HEIGHT 64
// #define RECT_X 100
// #define RECT_Y 60

// 	uint8_t rowBuf[RECT_WIDTH];
// 	for (int i = 0; i < RECT_WIDTH; ++i) {
// 		rowBuf[i] = VGA_COLOR_MAGENTA;
// 	}

// 	for (int i = 0; i < RECT_HEIGHT; ++i) {
// 		vga13_setrow(RECT_Y + i, RECT_X, RECT_WIDTH, rowBuf);
// 	}


	// uint8_t color = 0;

	// for (int y = 0; y < VGA13_HEIGHT; ++y) {
	// 	for (int x = 0; x < VGA13_WIDTH; ++x) {
	// 		vga13_setpixel(x, y, color++);
	// 	}
	// }

	return 0;
}

// #include <baseline/c_io.h>

// int main( void ) {
// 	c_puts( "Hello, world!\n" );
// 	return( 0 );
// }
