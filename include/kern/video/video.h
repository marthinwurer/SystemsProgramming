/*
** File: include/kern/video/video.h
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors: 
**
** Functions for the video module
*/


#ifndef _KERN_VIDEO_VIDEO_H
#define _KERN_VIDEO_VIDEO_H

#include <kern/graphics/text/PSFont.h>

#include <kern/vesa/VBEInfo.h>
#include <kern/vesa/VBEModeInfo.h>

#include <kern/video/VideoInfo.h>
#include <kern/video/VideoMode.h>
#include <kern/video/err.h>

#include <kern/video/globals.h>

#define VIDEO_FONTSET_GLYPHS 256
#define VIDEO_FONTSET_BYTES_PER_GLYPH 16
#define VIDEO_FONTSET_WIDTH 8
#define VIDEO_FONTSET_HEIGHT 16


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


/**
 * @brief Initializes the given font variable with the default fontset
 */
int video_defaultFont(PSFont *font);


//
// Dumps all members in the given VideoInfo block to the
// console.
//
int video_dumpInfo(VideoInfo *info);

/**
 * @brief Sets the video mode to the given mode instance.
 *
 * The video mode is changed by calling vbe_setMode, if successful, the
 * global variable VIDEO_MODE is then set and the framebuffer is cleared
 * with the color black.
 */
int video_setMode(VideoMode *mode);


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