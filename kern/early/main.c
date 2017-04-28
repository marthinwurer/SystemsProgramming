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

	errorcode = video_early_bestMode(VIDEO_MODE);
	if (errorcode != E_VIDEO_SUCCESS) {
		c_printf("Failed to find best mode. Code: %d\n", errorcode);
		return EXIT_VIDEO_ERROR;
	}

	c_printf("Using mode: 0x%x\n", VIDEO_MODE->modeNum);
	c_printf(" * Location: 0x%x\n", VIDEO_MODE->fb.location);
	c_printf(" * Width: %d\n", VIDEO_MODE->fb.width);
	c_printf(" * Height: %d\n", VIDEO_MODE->fb.height);
	c_printf(" * Scanline: %d\n", VIDEO_MODE->fb.pitch);
	c_printf(" * BPP: %d\n", VIDEO_MODE->fb.bpp);

	//__asm("hlt");

	VideoFb *fb = &VIDEO_MODE->fb;

	uint16_t vbeResult;
	if (vbe_setMode(VIDEO_MODE->modeNum | 0x4000, &vbeResult) != E_VESA_SUCCESS) {
		c_printf("Failed to switch mode. Code %d\n", vbeResult);
		return EXIT_VIDEO_ERROR;
	}

	__asm("hlt");

	return EXIT_SUCCESS;

}