
#include "draw.h"

#include <string.h>

#include <kern/graphics/text/text.h>
#include <kern/graphics/shapes/rect.h>

int vcon_vga_draw(VConCtrl *ctrl, VConChar *start, VConLine line, uint16_t row) {
	
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

int vcon_graphics_draw(VConCtrl *ctrl, VConChar *start, VConLine line, uint16_t row) {

	unsigned length = line.length >> VCON_LINE_LENGTH_LOC;

	PaintContext *ctx = &ctrl->ctx;
	unsigned fontWidth = ctx->font.width;
	unsigned fontHeight = ctx->font.height;
	Point p = { .x = 0, .y = row * fontHeight };
	
	VConChar ch;
	for (unsigned i = 0; i != length; ++i) {
		ch = *start++;
		graphics_drawChar(ctx, &p, ch.character);
		p.x += fontWidth;
	}

	Rect rect;
	rect.loc = p;
	rect.size.width = (ctrl->current->columns - length) * fontWidth;
	rect.size.height = fontHeight;

	ctx->fillCol = 0;
	graphics_fillRect(ctx, &rect);

	// VideoFb *fb = ctrl->ctx.fb;
	// unsigned rowEnd = p.y + ctrl->ctx.font.height;
	// unsigned row = p.y;

	// unsigned col = p.x;
	// unsigned colEnd = fb->width;

	// unsigned dcol = fb->bpp / 8;
	// unsigned pitch = fb->pitch;

	// uint32_t offset = fb_offset(fb, p.x, p.y);

	// do {
	// 	fb_putpixelf(fb, offset, 0);
	// 	if (++col == colEnd) {
	// 		col = 0;
	// 		++row;
	// 		offset += 
	// 	}
	// } while (row != rowEnd);

	return 0;

}