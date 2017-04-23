#ifndef _KERN_VIDEO_COLOR_COLOR_H
#define _KERN_VIDEO_COLOR_COLOR_H


VideoCol color_getComponent(VideoColorSpace *colorspace, VideoComp comp, int value);

VideoCol color_getColor(VideoColorSpace *colorspace, int red, int blue, int green);

int color_valueOf(VideoColorSpace *colorspace, VideoCol color);


#endif