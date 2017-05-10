#ifndef _KERN_VIDEO_COLOR_COLOR_H
#define _KERN_VIDEO_COLOR_COLOR_H

#include <kern/video/color/VideoColorSpace.h>
#include <kern/video/color/VideoComp.h>
#include <kern/video/color/VideoCol.h>

VideoCol color_getComponent(VideoColorSpace colorspace, VideoComp comp, int value);

VideoCol color_getColor(VideoColorSpace colorspace, int red, int green, int blue);

int color_valueOf(VideoColorSpace colorspace, VideoComp comp, VideoCol color);


#endif