/*
** File: include/kern/video/VideoStatus.h
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors: 
**
** Typedef for the VideoStatus enum
*/

#ifndef _KERN_VIDEO_VIDEOSTATUS_H
#define _KERN_VIDEO_VIDEOSTATUS_H

typedef enum VideoStatus_e {

	VIDEO_VBE_SUPPORT  = 0x1,    // bit 0 is set if VBE supported by this system
	VIDEO_EDID_SUPPORT = 0x2,    // bit 1 is set if EDID supported by this system
	VIDEO_TRUNCATED    = 0x4,    // bit 2 set if VIDEO_INFO was truncated to fit
	VIDEO_MODE_READY   = 0x8     // bit 3 is set if the video mode is set and ready

} VideoStatus;

#endif