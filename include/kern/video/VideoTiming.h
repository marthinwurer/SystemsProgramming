/*
** File: include/kern/video/VideoTiming.h
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors: 
**
** Typedef for the VideoTiming struct
*/

#ifndef _KERN_VIDEO_VIDEOTIMING_H
#define _KERN_VIDEO_VIDEOTIMING_H

typedef struct VideoTiming_s {

	uint16_t width;
	uint16_t height;
	uint8_t refresh;

} VideoTiming;


#endif