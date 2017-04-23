#ifndef _KERN_VIDEO_FB_VIDEOFB_H
#define _KERN_VIDEO_FB_VIDEOFB_H

#include <kern/video/color/VideoColorSpace.h>

#include <stdint.h>



//
// Struct typedef that encapsulates a linear framebuffer for the system
// Only graphic modes supported.
//
typedef struct VideoFb_s {
	uint32_t location; // physical address of the framebuffer
	uint16_t width;
	uint16_t height;
	uint16_t pitch;
	uint8_t bpp;
	VideoColorSpace colorspace;

} VideoFb;


#endif