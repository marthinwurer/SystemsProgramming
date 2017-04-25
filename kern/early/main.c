#include <kern/early/realmode.h>
#include <kern/vesa/vbe.h>
#include <kern/video/video.h>
#include <baseline/c_io.h>
#include <stddef.h>

#include <kern/video/fb/fb.h>
#include <kern/vesa/edid.h>

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

	/*c_puts("Obtaining VBE Controller information\n");
	VBEInfo info;
	uint16_t vbeResult;
	if (vbe_getInfo(&info, &vbeResult) == VBE_SUCCESS) {
		video_convertVBEInfo(&info, (VideoInfo*)VIDEO_INFO);

		VBEModeInfo modeInfo;
		uint16_t modeNum = -1;
		int count = 0;
		for (int i = 0; i != VIDEO_INFO->info.modeCount; ++i) {
			modeNum = VIDEO_INFO->info.modes[i];
			if (vbe_getModeInfo(modeNum, &modeInfo, NULL) == VBE_SUCCESS) {
				if ((modeInfo.v3.ModeAttributes & 144) == 144) {
					c_printf("0x%x: %dx%dx%d (%x)\n", modeNum,
					                                  modeInfo.v3.XResolution,
					                                  modeInfo.v3.YResolution,
													  modeInfo.v3.BitsPerPixel,
													  modeInfo.v3.MemoryModel);
					if (modeInfo.v3.BitsPerPixel == 32 && modeInfo.v3.MemoryModel == 0x6) {
						break;
					}
				}
				// if (modeInfo.v3.BitsPerPixel == 24 && (modeInfo.v3.ModeAttributes & 144) == 144
				//     && modeInfo.v3.MemoryModel == 0x6) {
				// 		break;
				// 	}
			}
		}

		regs16_t regs;
		regs.eax = 0;
		int32(0x16, &regs);

		if (modeNum != -1 && vbe_setMode(modeNum, NULL) == VBE_SUCCESS) {

		VideoFb fb;
		fb.location = modeInfo.v3.PhysBasePtr;
		fb.width = modeInfo.v3.XResolution;
		fb.height = modeInfo.v3.YResolution;
		fb.bpp = modeInfo.v3.BitsPerPixel;
		fb.pitch = modeInfo.v3.LinBytesPerScanLine;

		//*((uint16_t*)fb.location) = 0xFF00;
		//*((uint8_t*)(fb.location + 2)) = 0x00;

		fb_clear(&fb, 0xFF0000);
		}


		//c_printf("Avaliable modes: %d    Supported Modes: %d\n", VIDEO_INFO->info.modeCount, count);
	} else {
		c_printf("[ERROR] vbe_getInfo failed. %AX: %x\n", vbeResult);
		return 1;
	}*/

	c_puts("Getting EDID record...");

	EDIDRecord record;
	if (edid_getRecord(&record) == 0) {
		c_puts("Success\nHeader:");
		for (int i = 0 ; i != 8; ++i) {
			c_printf(" %x", record.Header[i]);
		}
		c_putchar('\n');

		c_printf("Manufacturer: %x    Product: %x    Serial: %d\n", record.ManufactureId, record.ProductCode, record.SerialCode);
		c_printf("Week: %d    Year: %d\n", record.ManufactureWeek, record.ManufactureYear);

		uint16_t timing;
		for (int i = 0; i != 8; ++i) {
			c_printf("Standard Timing %d: ", i + 1);
			timing = record.StandardTimings[i];
			c_printf("HAP: %d    ", timing & 0xFF);
			c_printf("AR: %d    ", (timing >> 14) & 3);
			c_printf("RR: %d\n", ((timing >> 8) & 63) + 60);

		}

	} else {
		c_puts("Failed\n");
	}

	__asm("hlt");

	return 0;

}