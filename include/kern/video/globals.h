/*
** File: include/kern/video/globals.h
**
** Author: Brennan Ringey (bjr1251)
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



/**
 * @brief Constant global pointer to the VideoInfo structure.
 *
 * Contains information returned by vbe_getInfo in a compact format.
 */
extern VideoInfo const *VIDEO_INFO;


/**
 * @brief Global pointer to the EDID Record, if it exists.
 *
 * This pointer is NULL if edid_getRecord failed.
 */
 extern EDIDRecord const *VIDEO_EDID;


/**
 * @brief Global constant pointer to the current VideoMode.
 */
extern VideoMode const *VIDEO_MODE;


#endif