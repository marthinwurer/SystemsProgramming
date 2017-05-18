/*
** File: kern/video/fb/fb.c
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors:
**
** Implementations for all fb_* functions
**
*/

#include <kern/video/fb/fb.h>
#include <kern/video/err.h>

#include <stddef.h>
#include <stdint.h>


int fb_init(VideoFb *fb) {
	(void)fb;
	return E_VIDEO_SUCCESS;
}

int fb_clear(VideoFb *fb, VideoCol color) {

	if (fb == NULL) {
		return E_VIDEO_ARGNULL;
	}

	uint32_t offset = 0;

	// length of a pixel in bytes
	uint16_t dcol = fb->bpp / 8;

	// distance in bytes from the end of the last pixel in a scanline to the
	// next scanline
	uint16_t dpitch = fb->pitch - (fb->width * dcol);


	uint16_t col = 0;
	uint16_t colEnd = fb->width;

	uint16_t row = 0;
	uint16_t rowEnd = fb->height;
	do {

		fb_putpixelf(fb, offset, color);
		offset += dcol; // go to next column
		//fb_offsetIncr(fb, &offset, 1, 0); // go to next column
		if (++col == colEnd) {
			col = 0;
			++row;
			offset += dpitch; // go to start of next scanline
		}

	} while (row != rowEnd);


	return E_VIDEO_SUCCESS;
}

int fb_clearcolsf(VideoFb *fb, uint32_t offset, uint16_t cols, VideoCol col) {

	unsigned dcol = fb->bpp / 8;
	for (unsigned c = 0; c != cols; ++c) {
		fb_putpixelf(fb, offset, col);
		offset += dcol;
	}

	return E_VIDEO_SUCCESS;
}

int fb_offset(VideoFb *fb, uint32_t *offset, uint16_t x, uint16_t y) {
	if (fb == NULL || offset == NULL) {
		return E_VIDEO_ARGNULL;
	}

	*offset = (y * fb->pitch) + (x * (fb->bpp / 8));
	
	return E_VIDEO_SUCCESS;
}

int fb_putpixel(VideoFb *fb, uint16_t x, uint16_t y, VideoCol color) {
	uint32_t offset;
	fb_offset(fb, &offset, x, y);
	return fb_putpixelf(fb, offset, color);
}

int fb_putcols(VideoFb *fb, uint16_t x, uint16_t y, uint16_t bufsize, VideoCol buf[]) {
	uint32_t offset;
	fb_offset(fb, &offset, x, y);
	return fb_putcolsf(fb, offset, bufsize, buf);
}

int fb_putrow(VideoFb *fb, uint16_t row, VideoCol buf[]) {
	uint32_t offset;
	fb_offset(fb, &offset, 0, row);
	return fb_putrowf(fb, offset, buf);
}

int fb_putpixelf(VideoFb *fb, uint32_t offset, VideoCol color) {

	if (fb == NULL) {
		return E_VIDEO_ARGNULL;
	}

	uint32_t location = fb->location + offset;

	unsigned pixlen = fb->bpp / 8;
	switch (pixlen) {
		case 0:
		case 1:
			*((uint8_t*)location) = (uint8_t)color;
			break;
		case 2:
			*((uint16_t*)location) = (uint16_t)color;
			break;
		case 3:
			*((uint16_t*)location) = (uint16_t)color;
			*((uint8_t*)(location + 2)) = (uint8_t)(color >> 16);
			break;
		case 4:
			*((uint32_t*)location) = (uint32_t)color;
			break;
		default:
			break;
	}


	return E_VIDEO_SUCCESS;
}

int fb_putcolsf(VideoFb *fb, uint32_t offset, uint16_t bufsize, VideoCol buf[]) {
	if (fb == NULL) {
		return E_VIDEO_ARGNULL;
	}

	unsigned dcol = fb->bpp / 8;
	for (unsigned i = 0; i != bufsize; ++i) {
		fb_putpixelf(fb, offset, buf[i]);
		offset += dcol;
	}


	return E_VIDEO_SUCCESS;
}

int fb_putrowf(VideoFb *fb, uint32_t offset, VideoCol buf[]) {
	if (fb == NULL) {
		return E_VIDEO_ARGNULL;
	}

	fb_putcolsf(fb, offset, fb->width, buf);

	return E_VIDEO_SUCCESS;
}
