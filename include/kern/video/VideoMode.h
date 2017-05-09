#ifndef _KERN_VIDEO_VIDEOMODE_H
#define _KERN_VIDEO_VIDEOMODE_H

#include <kern/video/fb/VideoFb.h>

typedef struct VideoMode_s {
	uint16_t modeNum;
	VideoFb fb;
} VideoMode;


#endif