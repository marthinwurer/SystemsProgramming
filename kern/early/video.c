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

#include <kern/video/video.h>
#include <kern/video/globals.h>

#include <kern/early/video.h>
#include <kern/video/err.h>

#include <kern/vesa/vbe.h>
#include <kern/vesa/edid.h>
#include <kern/vesa/err.h>

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
		if ((status & VIDEO_EDID_SUPPORT) == VIDEO_EDID_SUPPORT) {

		}

		// Matching a mode using EDID has failed, pick a "safe" resolution
		int index;
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