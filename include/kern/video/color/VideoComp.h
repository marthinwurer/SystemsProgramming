/*
** File: include/kern/video/color/VideoComp.h
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors: 
**
** Enum definition for the supported color components
*/

#ifndef _KERN_VIDEO_FB_VIDEOCOMP_H
#define _KERN_VIDEO_FB_VIDEOCOMP_H

typedef enum VideoComp_e {

	VIDEO_COMPONENT_RED = 0,
	VIDEO_COMPONENT_V = 0,

	VIDEO_COMPONENT_GREEN = 1,
	VIDEO_COMPONENT_U = 1,

	VIDEO_COMPONENT_BLUE = 2,
	VIDEO_COMPONENT_Y = 2

} VideoComp;

#define VIDEO_MAX_COMPONENTS 3


#endif