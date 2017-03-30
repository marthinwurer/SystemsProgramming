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


int main(void) {

	__init_interrupts();

	vga13_init();

	uint8_t rowBuf[VGA13_WIDTH];
	uint8_t color = 0;
	for (int i = 0; i != VGA13_WIDTH; ++i) {
		rowBuf[i] = color++;
	}

	for (int y = 0; y != VGA13_HEIGHT; ++y) {
		vga13_setrow(y, 0, VGA13_WIDTH, rowBuf);
	}


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
