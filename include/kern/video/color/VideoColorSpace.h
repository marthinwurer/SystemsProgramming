#ifndef _KERN_VIDEO_FB_VIDEOCOLORSPACE_H
#define _KERN_VIDEO_FB_VIDEOCOLORSPACE_H

#include <kern/video/color/VideoComp.h>

#include <stdint.h>

struct VideoColorConfig_s {
	uint8_t mask;
	uint8_t position;
};


typedef struct VideoColorConfig_s VideoColorSpace[VIDEO_MAX_COMPONENTS];


#endif