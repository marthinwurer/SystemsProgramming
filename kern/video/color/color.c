/*
** File: kern/video/color/color.c
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors:
**
** Implementations for all color_* functions
**
*/

#include <kern/video/color/color.h>

VideoCol color_getComponent(VideoColorSpace colorspace, VideoComp comp, int value) {
	struct VideoColorConfig_s config = colorspace[comp];

	return (value & config.mask) << config.position;
	//return (value << config.position) & config.mask;
}

VideoCol color_getColor(VideoColorSpace colorspace, int red, int green, int blue) {

	return color_getComponent(colorspace, VIDEO_COMPONENT_RED, red) |
	       color_getComponent(colorspace, VIDEO_COMPONENT_GREEN, green) |
	       color_getComponent(colorspace, VIDEO_COMPONENT_BLUE, blue);

}

int color_valueOf(VideoColorSpace colorspace, VideoComp comp, VideoCol color) {
	struct VideoColorConfig_s config = colorspace[comp];

	return (color >> config.position) & config.mask;
}
