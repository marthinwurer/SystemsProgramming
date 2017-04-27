#ifndef _KERN_VIDEO_VIDEO_H
#define _KERN_VIDEO_VIDEO_H

#include <kern/vesa/VBEInfo.h>
#include <kern/vesa/VBEModeInfo.h>

#include <kern/video/VideoInfo.h>
#include <kern/video/VideoMode.h>
#include <kern/video/err.h>

// location in memory of the VIDEO_INFO global variable
#define VIDEO_INFO_ADDRESS 0x3800

// memory used by video_init when determining a VBE mode
#define VIDEO_TEMP_ADDRESS 0x3A00

extern VideoInfo const *VIDEO_INFO;


/**
 * @brief Selects and changes to the best available video mode.
 */
int video_init(void);

//
// Converts the VBEInfo structure into a VideoInfo structure
// using the given pointers.
//
int video_convertVBEInfo(VBEInfo *vbe, VideoInfo *info);


int video_convertVBEMode(VBEModeInfo *vbe, VideoMode *mode);

//
// Dumps all members in the given VideoInfo block to the
// console.
//
int video_dumpInfo(VideoInfo *info);


/**
 * @brief Changes the video mode to 720x480, that most monitors support.
 *
 * This function is used when getting the EDID of a monitor fails.
 */
//int video_fallbackVESA()

/**
 * @brief Changes the video mode to VGA Mode 0x13 as a fallback.
 *
 * This mode should only be used when all else has failed.
 */
//int video_fallbackVGA();




#endif