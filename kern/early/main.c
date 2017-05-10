#include <kern/early/realmode.h>
#include <kern/early/video.h>
#include <kern/vesa/vbe.h>
#include <kern/video/video.h>
#include <baseline/c_io.h>
#include <stddef.h>

#include <kern/early/memory_map_setup.h>

#include <kern/video/color/color.h>
#include <kern/video/fb/fb.h>
#include <kern/vesa/edid.h>
#include <kern/vesa/err.h>

#include <kern/vconsole/console.h>
#include <kern/vconsole/control.h>


// Exit codes

#define EXIT_VIDEO_ERROR 1
#define EXIT_SUCCESS 0


//
// Main function for the early initialization routine. Any needed BIOS function
// calls should be done here while storing the results somewhere the kernel can
// access it. Any errors that occur here are to be handled by the kernel
// (unless the error is unrecoverable, then return nonzero).
//
// Nothing has been setup yet, so there should be no problems setting things
// up using the protected-to-real mode int32 function.
//
int main(void) {

	get_memory_map();

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
	c_printf(" * RGB: %d (%d) | %d (%d) | %d (%d)\n", mode.fb.colorspace[0].position,
	                                                  mode.fb.colorspace[0].mask,
													  mode.fb.colorspace[1].position,
	                                                  mode.fb.colorspace[1].mask,
													  mode.fb.colorspace[2].position,
	                                                  mode.fb.colorspace[2].mask);

	// vcon_scroll(CIO_CONTROLLER.current, 2);

	// vcon_redraw(&CIO_CONTROLLER);

	// __asm("hlt");

	// Set the mode

	errorcode = video_setMode(&mode);
	if (errorcode != E_VIDEO_SUCCESS) {
		c_printf("Failed to switch mode. Code %d\n", errorcode);
		return EXIT_VIDEO_ERROR;
	}

	CIO_CONTROLLER.mode = VCON_MODE_GRAPHICS;
	PaintContext *ctx = &CIO_CONTROLLER.ctx;
	ctx->drawCol = color_getColor(mode.fb.colorspace, 255, 255, 255);
	ctx->fillCol = color_getColor(mode.fb.colorspace, 0, 0, 0);
	// ctx->font.glyphs = 256;
	// ctx->font.bytesPerGlyph = 16;
	// ctx->font.height = 16;
	// ctx->font.width = 8;
	// ctx->font.glyphMap = VIDEO_FONTSET;
	video_defaultFont(&ctx->font);
	ctx->fb = &VIDEO_MODE->fb;

	CIO_CONTROLLER.current->columns = mode.fb.width / ctx->font.width;
	CIO_CONTROLLER.current->rows = mode.fb.height / ctx->font.height;

	vcon_redraw(&CIO_CONTROLLER);

	//__asm("hlt");

	return EXIT_SUCCESS;

}