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


int video_early_init(void) {

	video_init();

	VBEInfo info;
	uint16_t vbeResult;
	if (vbe_getInfo(&info, &vbeResult) == E_VESA_SUCCESS) {
		if (video_convertVBEInfo(&info, VIDEO_INFO) == E_VIDEO_INFO_TRUNCATED) {
			c_puts("video: info block truncated\n");
		}
	} else {
		c_printf("video: Failed to get VBE info (%d)\n", vbeResult);
	}

	int edidResult = edid_getRecord(VIDEO_EDID);
	if (edidResult != E_VESA_SUCCESS) {
		c_printf("video: Failed to get EDID record (%d)\n", edidResult);
	}

	return E_VIDEO_SUCCESS;
}


int video_early_bestMode(VideoMode *mode) {

	return E_VIDEO_SUCCESS;
}