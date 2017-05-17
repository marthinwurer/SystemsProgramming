/*
** File: kern/early/video.c
**
** Author: Brennan Ringey (bjr1251)
**
** Contains function implementations defined in include/kern/early/video.h
**
** The functions defined here are for early initialization for the video
** module. 
**
*/
#include <baseline/c_io.h>

#include <kern/video/video.h>
#include <kern/video/globals.h>

#include <kern/early/video.h>
#include <kern/video/err.h>

#include <kern/vesa/vbe.h>
#include <kern/vesa/edid.h>
#include <kern/vesa/err.h>

#include <kern/early/realmode.h>

#include <stddef.h>


static VideoTiming SAFE_TIMINGS[] = {
	{ .width = 720, .height = 480, .refresh = 0},
	{ .width = 640, .height = 480, .refresh = 0},
	{ .width = 360, .height = 480, .refresh = 0},
	{ .width = 320, .height = 480, .refresh = 0},
	{ .width = 720, .height = 240, .refresh = 0},
	{ .width = 640, .height = 240, .refresh = 0},
	{ .width = 360, .height = 240, .refresh = 0},
	{ .width = 320, .height = 240, .refresh = 0}
};

#define N_SAFE_TIMINGS (sizeof(SAFE_TIMINGS) / sizeof(VideoTiming))


static int __matchMode(VideoMode *modeList, unsigned size, VideoTiming timing);


int video_early_init(void) {

	video_init();

	VBEInfo info;
	uint16_t vbeResult;
	if (vbe_getInfo(&info, &vbeResult) == E_VESA_SUCCESS) {
		VIDEO_INFO->info.status |= VIDEO_VBE_SUPPORT;
		if (video_convertVBEInfo(&info, VIDEO_INFO) == E_VIDEO_INFO_TRUNCATED) {
			VIDEO_INFO->info.status |= VIDEO_TRUNCATED;
			c_puts("video: info block truncated\n");
		}
	} else {
		c_printf("video: Failed to get VBE info (%d)\n", vbeResult);
	}

	int edidResult = edid_getRecord(VIDEO_EDID);
	if (edidResult == E_VESA_SUCCESS) {
		VIDEO_INFO->info.status |= VIDEO_EDID_SUPPORT;
	} else {
		c_printf("video: Failed to get EDID record (%d)\n", edidResult);
	}

	// Now copy the VGA fontset someplace so we don't have to make one
	// ourselves. Example taken from http://wiki.osdev.org/VGA_Fonts

	regs16_t regs = {0};
	regs.eax = 0x1130;
	regs.ebx = 0x0600;
	int32(0x10, &regs);

	uint32_t *srcFontset = (uint32_t*)((regs.es << 4) + regs.ebp);
	uint32_t *dstFontset = (uint32_t*)VIDEO_FONTSET_ADDRESS;

	for (int i = 0; i != 1024; ++i) {
		//for (int j = 0; j != 4; ++j) {
			*dstFontset++ = *srcFontset++;
		//}
	}

	// __outb(0x3CE, 5); // clear even/odd mode
	// __outb(0x3CE, 0x406); // map VGA memory to 0xA0000
	// __outb(0x3C4, 0x402); // set bitplane 2
	// __outb(0x3C4, 0x604); // clear even/odd mode again

	// uint32_t *dstFontset = (uint32_t*)0x3C00;
	// uint32_t *srcFontset = (uint32_t*)0xA0000;

	// // copy charmap
	// for (int i = 0; i != 256; ++i) {
	// 	// copy 16-byte glyph to destination fontset
	// 	for (int j = 0; j != 4; ++j) {
	// 		*dstFontset++ = *srcFontset++;
	// 	}
	// 	srcFontset += 4; // skip 16 bytes
	// }

	// // reset VGA state
	// __outb(0x3C4, 0x302);
	// __outb(0x3C4, 0x204);
	// __outb(0x3CE, 0x1005);
	// __outb(0x3CE, 0xE06);



	return E_VIDEO_SUCCESS;
}


int video_early_bestMode(VideoMode *mode) {

	// desired attributes:
	// 1. Supported by hardware
	// 2. Color support (not monochrome)
	// 3. Graphics mode
	// 4. Has linear framebuffer
	#define DESIRED_ATTRIBUTES (VBE_MODEATTR_SUPPORTED \
	                            | VBE_MODEATTR_COLOR \
	                            | VBE_MODEATTR_GRAPHICS \
	                            | VBE_MODEATTR_FRAMEBUFFER)

	VideoStatus status = VIDEO_INFO->info.status;

	if ((status & VIDEO_VBE_SUPPORT) == VIDEO_VBE_SUPPORT) {

		VBEModeInfo modeInfo;
		unsigned modeCount = VIDEO_INFO->info.modeCount;
		VideoMode modeList[modeCount];
		unsigned usableModes = 0;
		for (unsigned i = 0; i != modeCount; ++i) {
			uint16_t curModeNum = VIDEO_INFO->info.modes[i];
			if (vbe_getModeInfo(curModeNum, &modeInfo, NULL) == E_VESA_SUCCESS) {
				if ((modeInfo.v3.ModeAttributes & DESIRED_ATTRIBUTES) == DESIRED_ATTRIBUTES
				    && modeInfo.v3.MemoryModel == VBE_MODEL_DIRECT
				    && modeInfo.v3.BitsPerPixel >= 24) {

						video_convertVBEMode(&modeInfo, modeList + usableModes);
						(modeList + usableModes)->modeNum = curModeNum;
						++usableModes;
				}
			}
		}

		int index;

		if ((status & VIDEO_EDID_SUPPORT) == VIDEO_EDID_SUPPORT) {
			// try to match a vbe mode with a detailed timing entry
			VideoTiming timing;
			for (int i = 0; i != EDID_DETAILED_TIMING_COUNT; ++i) {
				if (edid_parseDetailed(VIDEO_EDID, &timing, i) == E_VESA_SUCCESS) {
					index = __matchMode(modeList, usableModes, timing);
					if (index != -1) {
						*mode = modeList[index];
						return E_VIDEO_SUCCESS;
					}
				}
			}

			for (int i = 0; i != EDID_STANDARD_TIMING_COUNT; ++i) {
				if (edid_parseStandard(VIDEO_EDID, &timing, i) == E_VESA_SUCCESS) {
					index = __matchMode(modeList, usableModes, timing);
					if (index != -1) {
						*mode = modeList[index];
						return E_VIDEO_SUCCESS;
					}
				}
			}
		}

		// Matching a mode using EDID has failed, pick a "safe" resolution
		
		for (unsigned i = 0; i != N_SAFE_TIMINGS; ++i) {
			index = __matchMode(modeList, usableModes, SAFE_TIMINGS[i]);
			if (index != -1) {
				*mode = modeList[index];
				return E_VIDEO_SUCCESS;
			}
		}


	}
	
	#undef DESIRED_ATTRIBUTES
	return E_VIDEO_UNSUPPORTED; // did not find a mode
}


int __matchMode(VideoMode *modeList, unsigned size, VideoTiming timing) {

	VideoMode *mode;
	for (unsigned i = 0; i != size; ++i) {
		mode = modeList + i;
		if (mode->fb.width == timing.width && mode->fb.height == timing.height) {
			return i;
		}
	}

	return -1;

}