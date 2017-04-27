/*
** File: kern/early/video.c
**
** Author: Brennan Ringey (bjr1251)
**
** Contains function implementations defined in include/kern/early/video.h
**
** The functions defined here are for early initialization for the video
** module. 
**
*/


#include <kern/early/video.h>
#include <kern/video/err.h>


int video_early_init(void) {

	return E_VIDEO_SUCCESS;
}


int video_early_bestMode(VideoMode *mode) {

	return E_VIDEO_SUCCESS;
}