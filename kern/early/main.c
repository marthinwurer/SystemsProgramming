#include <kern/early/realmode.h>
#include <kern/vesa/vbe.h>
#include <kern/video/video.h>
#include <baseline/c_io.h>
#include <stddef.h>

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

	c_puts("Obtaining VBE Controller information\n");
	VBEInfo info;
	uint16_t vbeResult;
	if (vbe_getInfo(&info, &vbeResult) == VBE_SUCCESS) {
		video_convertVBEInfo(&info, (VideoInfo*)VIDEO_INFO);
	} else {
		c_printf("[ERROR] vbe_getInfo failed. %AX: %x\n", vbeResult);
		return 1;
	}
	//__asm("hlt");

	return 0;

}