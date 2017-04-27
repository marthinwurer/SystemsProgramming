/*
** File: kern/video/globals.c
**
** Author: Brennan Ringey (bjr1251)
**
** Contains global variable initialization for the video module.
*/

#include <kern/video/globals.h>

VideoInfo const *VIDEO_INFO = (VideoInfo*)VIDEO_INFO_ADDRESS;

EDIDRecord const *VIDEO_EDID = (EDIDRecord*)VIDEO_EDID_ADDRESS;

VideoMode const *VIDEO_MODE = (VideoMode*)VIDEO_MODE_ADDRESS;
