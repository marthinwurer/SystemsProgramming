/*
** File: kern/vga13/vga13.c
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors:
**
** DEPRECATED!
** VGA mode 0x13 driver implementation.
**
*/


#include <kern/vga13/vga13.h>
#include <kern/vgacolor.h>
#include <kern/vga13/err.h>

// true for invalid bounds, false otherwise
#define BOUNDS_CHECK(x,y) (x >= VGA13_WIDTH || y >= VGA13_HEIGHT)

#define NULL ((void*)0)

static uint8_t *framebuffer = VGA13_FRAMEBUFFER;


int vga13_clear(uint8_t color) {

	// initialize the row buffer with the color to clear with
	uint8_t rowBuf[VGA13_WIDTH];
	for (int col = 0; col != VGA13_WIDTH; ++col) {
		rowBuf[col] = color;
	}

	for (int row = 0; row != VGA13_HEIGHT; ++row) {
		vga13_setRow(row, rowBuf);
	}

	return E_SUCCESS;
}

int vga13_getColumns(unsigned row, unsigned col, unsigned bufsize, uint8_t buf[]) {

	if (BOUNDS_CHECK(col, row) || (col + bufsize) > VGA13_WIDTH) {
		return E_VGA13_BOUNDS;
	}

	if (buf == NULL) {
		return E_VGA13_ARGNULL;
	}

	int offset = VGA13_offset(col, row);
	for (unsigned c = 0; c != bufsize; ++c) {
		buf[c] = framebuffer[offset + c];
	}

	return E_SUCCESS;
}

int vga13_getPixel(unsigned x, unsigned y, uint8_t *pixelVar) {
	if (BOUNDS_CHECK(x, y)) {
		// error invalid bounds for x or y
		return E_VGA13_BOUNDS;
	}

	if (pixelVar == NULL) {
		return E_VGA13_ARGNULL;
	}

	*pixelVar = framebuffer[VGA13_offset(x, y)];
	return E_SUCCESS;
}

int vga13_getRow(unsigned row, uint8_t buf[]) {
	return vga13_getColumns(row, 0, VGA13_WIDTH, buf);	
}

int vga13_init(void) {
	vga13_clear(VGA_COLOR_BLACK);
	return E_SUCCESS;
}

int vga13_setPixel(unsigned x, unsigned y, uint8_t color) {
	if (BOUNDS_CHECK(x, y)) {
		return E_VGA13_BOUNDS;
	}

	framebuffer[VGA13_offset(x, y)] = color;
	return E_SUCCESS;
}

int vga13_setColumns(unsigned row, unsigned col, unsigned bufsize, uint8_t buf[]) {
	
	if (BOUNDS_CHECK(col, row) || (col + bufsize) > VGA13_WIDTH) {
		return E_VGA13_BOUNDS;
	}

	if (buf == NULL) {
		return E_VGA13_ARGNULL;
	}
	
	int offset = VGA13_offset(col, row);
	for (unsigned c = 0; c != bufsize; ++c) {
		framebuffer[offset + c] = buf[c];
	}

	return E_SUCCESS;
}

int vga13_setRow(unsigned row, uint8_t buf[]) {
	return vga13_setColumns(row, 0, VGA13_WIDTH, buf);
}

