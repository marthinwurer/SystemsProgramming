
#include "draw.h"

#include <string.h>

#include <kern/graphics/text/text.h>

int vga_draw(VConCtrl *ctrl, VConChar *start, VConLine line, uint16_t row) {
	
	uint16_t columns = ctrl->current->columns;
	unsigned length = line.length >> VCON_LINE_LENGTH_LOC;

	// draw the line
	uint16_t *dest = (void*)(0xB8000 + (row * columns * sizeof(VConChar)));
	memcpy(dest, start, sizeof(VConChar) * length);

	// clear the rest of the line
	dest += length;
	memset(dest, 0, (columns - length) * sizeof(VConChar));
	
	return 0;
}

int graphics_draw(VConCtrl *ctrl, VConChar *start, VConLine line, uint16_t row) {

	unsigned length = line.length >> VCON_LINE_LENGTH_LOC;

	Point p = { .x = 0, .y = row * ctrl->ctx.font.height };
	unsigned fontWidth = ctrl->ctx.font.width;

	VConChar ch;
	for (unsigned i = 0; i != length; ++i) {
		ch = *start++;
		graphics_drawChar(&ctrl->ctx, &p, ch.character);
		p.x += fontWidth;
	}

	return 0;


	return 0;
}