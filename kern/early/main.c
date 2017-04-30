#include <kern/early/realmode.h>
#include <kern/early/video.h>
#include <kern/vesa/vbe.h>
#include <kern/video/video.h>
#include <baseline/c_io.h>
#include <stddef.h>

#include <kern/video/fb/fb.h>
#include <kern/vesa/edid.h>
#include <kern/vesa/err.h>


// Exit codes

#define EXIT_VIDEO_ERROR 1
#define EXIT_SUCCESS 0


void __putchar(VideoFb *fb, int x, int y, VideoCol fg, VideoCol bg, char c) {

	uint8_t *glyph = (uint8_t*)(0x3C00 + (c * 16));
	VideoCol rowBuf[8];
	uint8_t row;

	for (int i = 0; i != 16; ++i) {
		row = glyph[i];
		for (int j = 0; j != 8; ++j) {
			if ((row >> (7 - j)) & 1) {
				rowBuf[j] = fg;
			} else {
				rowBuf[j] = bg;
			}
		}
		fb_putcols(fb, x, y + i, 8, rowBuf);
	}

}

//
// Main function for the early initialization routine. Any needed BIOS function
// calls should be done here while storing the results somewhere the kernel can
// access it. Note that you cannot use c_io since, like the bootstrap, it is
// not linked with that code. Any errors that occur here are to be handled 
// by the kernel (unless the error is unrecoverable, then just halt).
//
// Nothing has been setup yet, so there should be no problems setting things
// up using the protected-to-real mode int function.
//
int main(void) {

	// Initialize the video module

	int errorcode = video_early_init();
	if (errorcode != E_VIDEO_SUCCESS) {
		c_printf("Video initialization failed! Code: %d\n", errorcode);
		return EXIT_VIDEO_ERROR;
	}

	// Find the best available video mode

	VideoMode mode;
	errorcode = video_early_bestMode(&mode);
	if (errorcode != E_VIDEO_SUCCESS) {
		c_printf("Failed to find best mode. Code: %d\n", errorcode);
		return EXIT_VIDEO_ERROR;
	}

	// Dump the best mode

	c_printf("Using mode: 0x%x\n", mode.modeNum);
	c_printf(" * Location: 0x%x\n", mode.fb.location);
	c_printf(" * Width: %d\n", mode.fb.width);
	c_printf(" * Height: %d\n", mode.fb.height);
	c_printf(" * Scanline: %d\n", mode.fb.pitch);
	c_printf(" * BPP: %d\n", mode.fb.bpp);

	// Set the mode

	errorcode = video_setMode(&mode);
	if (errorcode != E_VIDEO_SUCCESS) {
		c_printf("Failed to switch mode. Code %d\n", errorcode);
		return EXIT_VIDEO_ERROR;
	}

	// white-on-purple test

	VideoCol white = color_getColor(mode.fb.colorspace, 255, 255, 255);
	VideoCol purple = color_getColor(mode.fb.colorspace, 75, 0, 130);

	fb_clear(&mode.fb, purple);

	// test draw character using fontset at 0x3C00
	int x, y;
	for (int i = 0; i != 256; ++i) {
		x = (i % 32) * 8;
		y = (i / 32) * 16;
		__putchar(&mode.fb, 16 + x, 16 + y, white, purple, (char)i);
	}

	//__putchar(&mode.fb, 10, 10, white, purple, 'B');

	__asm("hlt");

	return EXIT_SUCCESS;

}