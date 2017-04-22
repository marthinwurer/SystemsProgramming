#ifndef _KERN_VIDEO_VIDEO_H
#define _KERN_VIDEO_VIDEO_H

#include <kern/vesa/VBEInfo.h>

#include <kern/video/VideoInfo.h>
#include <kern/video/err.h>

// location in memory of the VIDEO_INFO global variable
#define VIDEO_INFO_ADDRESS 0x3800

extern VideoInfo const *VIDEO_INFO;


int video_init(void);

//
// Converts the VBEInfo structure into a VideoInfo structure
// using the given pointers.
//
int video_convertVBEInfo(VBEInfo *vbe, VideoInfo *info);

//
// Dumps all members in the given VideoInfo block to the
// console.
//
int video_dumpInfo(VideoInfo *info);




#endif