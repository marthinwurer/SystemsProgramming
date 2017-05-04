#include <kern/graphics/text/text.h>

#include <kern/video/fb/fb.h>


int graphics_drawString(PaintContext *ctx, Point *p, const char *string) {

	Point p2 = *p;
	int fontWidth = ctx->font.width;
	char c;
	while ((c = *string) != '\0') {
		graphics_drawChar(ctx, &p2, c);
		p2.x += fontWidth;
		++string;
	}

	return 0;
}

int graphics_drawChar(PaintContext *ctx, Point *p, char c) {
	uint8_t *glyph = ctx->font.glyphMap + (c * ctx->font.bytesPerGlyph);
	VideoCol rowBuf[ctx->font.width];
	uint8_t row;

	VideoCol fg = ctx->drawCol;
	VideoCol bg = ctx->fillCol;
	VideoFb *fb = ctx->fb;
	int x = p->x;
	int y = p->y;

	// calculating an offset from x and y is slow.
	// do it once and increment the offset as needed
	uint32_t offset;
	fb_offset(fb, &offset, x, y);

	for (unsigned i = 0; i != ctx->font.height; ++i) {
		row = glyph[i];
		for (int j = 0; j != 8; ++j) {
			if ((row >> (7 - j)) & 1) {
				rowBuf[j] = fg;
			} else {
				rowBuf[j] = bg;
			}
		}
		fb_putcolsf(fb, offset, 8, rowBuf);
		offset += fb->pitch;
	}

	return 0;
}