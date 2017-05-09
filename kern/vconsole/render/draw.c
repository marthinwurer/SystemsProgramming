
#include "draw.h"

#include <string.h>

int vga_draw(VCon *con, VConChar *start, VConLine line, uint16_t row) {
	
	unsigned length = line.length >> VCON_LINE_LENGTH_LOC;

	// draw the line
	uint16_t *dest = (void*)(0xB8000 + (row * con->columns * sizeof(VConChar)));
	memcpy(dest, start, sizeof(VConChar) * length);

	// clear the rest of the line
	dest += length;
	memset(dest, 0, (con->columns - length) * sizeof(VConChar));
	
	return 0;
}

int graphics_draw(VCon *con, VConChar *start, VConLine line, uint16_t row) {
	return 0;
}