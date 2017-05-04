#ifndef _KERN_VIDEO_VIDEOCOL_H
#define _KERN_VIDEO_VIDEOCOL_H

#include <stdint.h>

//
// typedef representing the data of a single pixel. The organization of
// the pixel's components depends on mask and pos fields in the VideoFb
// type. The size of this type is not the actual size of a pixel for a
// framebuffer. Use VideoFb.bpp instead.
//
typedef int32_t VideoCol;


#endif