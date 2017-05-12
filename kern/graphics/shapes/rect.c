#include <kern/graphics/shapes/rect.h>

#include <kern/video/fb/fb.h>

int graphics_drawRect(PaintContext *ctx, Rect *rect) {
	(void)ctx;
	(void)rect;

	return 0;
}

int graphics_fillRect(PaintContext *ctx, Rect *rect) {

	VideoCol col = ctx->fillCol;
	VideoFb *fb = ctx->fb;
	uint16_t pitch = fb->pitch;
	uint16_t rectWidth = rect->size.width;

	uint32_t offset;
	fb_offset(fb, &offset, rect->loc.x, rect->loc.y);

	for (uint16_t y = 0, end = rect->size.height; y != end; ++y) {
		fb_clearcolsf(fb, offset, rectWidth, col);
		offset += pitch;
	}

	return 0;
}