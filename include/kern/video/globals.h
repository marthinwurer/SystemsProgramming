/*
** File: include/kern/video/globals.h
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors: 
**
** Contains global variable definitions for the video module.
*/

#ifndef _KERN_VIDEO_GLOBALS_H
#define _KERN_VIDEO_GLOBALS_H

#include <kern/video/VideoInfo.h>
#include <kern/video/VideoMode.h>
#include <kern/vesa/EDIDRecord.h>


#define VIDEO_INFO_ADDRESS 0x3800
#define VIDEO_EDID_ADDRESS 0x3A00
#define VIDEO_MODE_ADDRESS 0x3B00

#define VIDEO_FONTSET_ADDRESS 0x3C00
#define VIDEO_FONTSET_SIZE 4096 // 256 8x16 bit glyphs


/**
 * @brief Constant global pointer to the VideoInfo structure.
 *
 * Contains information returned by vbe_getInfo in a compact format.
 */
extern VideoInfo *VIDEO_INFO;


/**
 * @brief Global pointer to the EDID Record, if it exists.
 *
 * This pointer is NULL if edid_getRecord failed.
 */
 extern EDIDRecord *VIDEO_EDID;


/**
 * @brief Global constant pointer to the current VideoMode.
 */
extern VideoMode *VIDEO_MODE;

/**
 * @brief Global pointer to the default fontset, the BIOS fontset.
 *
 * This pointer points to a block of 4096 bytes in memory containing a copy
 * of the BIOS 8x16 fontset. This fontset contains 256 glyphs, with each glyph
 * 8x16 pixels in size. A single glyph is 16 bytes. To access the corresponding
 * ASCII glyph, take the ascii value and multiply by 16, then add this value to
 * the pointer.
 */
extern uint8_t *VIDEO_FONTSET;


#endif