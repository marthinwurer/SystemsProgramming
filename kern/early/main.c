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
	if (vbe_getInfo(&info, NULL) == VBE_SUCCESS) {
		video_convertVBEInfo(&info, (VideoInfo*)VIDEO_INFO);
		
		//c_printf("%d\n", i);
		//c_printf("%x %x %x %x %x", VIDEO_INFO.info.modes, VIDEO_INFO.info.oem, VIDEO_INFO.info.vendor, VIDEO_INFO.info.productName, VIDEO_INFO.info.productRev);
		// c_printf(" * Version: %x\n", info.version);
		// c_printf(" * Revision: %d\n", info.softwareRev);
		// c_printf(" * Video Memory: %d 64KB blocks\n", info.videoMemory);
		// c_printf(" * OEM: %s (0x%x)\n", (const char *)vbe_ptr(info.oem), info.oem);
		// c_printf(" * Vendor: %s (0x%x)\n", (const char *)vbe_ptr(info.vendor), info.vendor);
		// c_printf(" * Product: %s (0x%x)\n", (const char *)vbe_ptr(info.productName), info.productName);
	}
	//__asm("hlt");

	return 0;

}