#include <kern/graphics/text/text.h>

#include <kern/video/fb/fb.h>

// static VideoCol MASK_TABLE[] = {
// 	/*  0: */          0,          0,          0,          0
// 	/*  1: */          0,          0,          0, 0xFFFFFFFF,
// 	/*  2: */          0,          0, 0xFFFFFFFF,          0,
// 	/*  3: */          0,          0, 0xFFFFFFFF, 0xFFFFFFFF,
// 	/*  4: */          0, 0xFFFFFFFF,          0,          0,
// 	/*  5: */          0, 0xFFFFFFFF,          0, 0xFFFFFFFF,
// 	/*  6: */          0, 0xFFFFFFFF, 0xFFFFFFFF,          0,
// 	/*  7: */          0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
// 	/*  8: */ 0xFFFFFFFF,          0,          0,          0,
// 	/*  9: */ 0xFFFFFFFF,          0,          0, 0xFFFFFFFF,
// 	/* 10: */ 0xFFFFFFFF,          0, 0xFFFFFFFF,          0,
// 	/* 11: */ 0xFFFFFFFF,          0, 0xFFFFFFFF, 0xFFFFFFFF,
// 	/* 12: */ 0xFFFFFFFF, 0xFFFFFFFF,          0,          0,
// 	/* 13: */ 0xFFFFFFFF, 0xFFFFFFFF,          0, 0xFFFFFFFF,
// 	/* 14: */ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,          0,
// 	/* 15: */ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
	

// };


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