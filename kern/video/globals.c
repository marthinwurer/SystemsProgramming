/*
** File: kern/video/globals.c
**
** Author: Brennan Ringey (bjr1251)
**
** Contains global variable initialization for the video module.
*/

#include <kern/video/globals.h>

VideoInfo *VIDEO_INFO = (VideoInfo*)VIDEO_INFO_ADDRESS;

EDIDRecord *VIDEO_EDID = (EDIDRecord*)VIDEO_EDID_ADDRESS;

VideoMode *VIDEO_MODE = (VideoMode*)VIDEO_MODE_ADDRESS;

uint8_t *VIDEO_FONTSET = (uint8_t*)VIDEO_FONTSET_ADDRESS;