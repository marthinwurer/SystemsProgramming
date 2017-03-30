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
		vga13_setrow(row, 0, VGA13_WIDTH, rowBuf);
	}

	return E_SUCCESS;
}

int vga13_getColumns(unsigned row, unsigned from, unsigned to, uint8_t *buf) {
	int columnCount = to - from;
	// 0 <= row < VGA13_HEIGHT
	// 0 <= from < VGA13_WIDTH
	// from <= to <= VGA13_WIDTH
	if (BOUNDS_CHECK(from, row) || columnCount < 0 || to > VGA13_WIDTH) {
		return E_VGA13_BOUNDS;
	}

	if (buf == NULL) {
		return E_VGA13_ARGNULL;
	}

	int offset = VGA13_offset(from, row);
	for (int c = 0; c != columnCount; ++c) {
		buf[c] = framebuffer[offset + c];
	}

	return E_SUCCESS;
}

int vga13_init(void) {
	vga13_clear(VGA_COLOR_BLACK);
	return E_SUCCESS;
}

int vga13_getpixel(unsigned x, unsigned y, uint8_t *pixelVar) {
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

int vga13_setpixel(unsigned x, unsigned y, uint8_t color) {
	if (BOUNDS_CHECK(x, y)) {
		return E_VGA13_BOUNDS;
	}

	framebuffer[VGA13_offset(x, y)] = color;
	return E_SUCCESS;
}

int vga13_getrow(unsigned row, unsigned col, unsigned cols, uint8_t *buf) {

	
}

int vga13_setrow(unsigned row, unsigned col, unsigned cols, uint8_t *buf) {
	
	if (BOUNDS_CHECK(col, row) || cols > VGA13_WIDTH) {
		return E_VGA13_BOUNDS;
	}

	if (buf == NULL) {
		return E_VGA13_ARGNULL;
	}
	
	int offset = VGA13_offset(col, row);
	for (int c = 0; c != cols; ++c) {
		framebuffer[offset + c] = buf[c];
	}

	return E_SUCCESS;
}

