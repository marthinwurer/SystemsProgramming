/*
** File: include/kern/early/video.h
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors:
**
** Contains early initialization functions for the video module.
*/

#ifndef _KERN_EARLY_VIDEO_H
#define _KERN_EARLY_VIDEO_H


#include <kern/video/VideoMode.h>

/**
 * @brief Initializes all global variables in the video module.
 *
 * Global variables in the video module that contain information obtainable
 * only in early init are set by this function. The VBE Controller information
 * as well as the EDID record are obtained and set by this function.
 */
int video_early_init(void);


/**
 * @brief Determines the best available mode using VBE/EDID information.
 *
 * This function uses the variables set by video_early_init to determine the
 * best available video mode. There are a few possibilities for the mode
 * selected and are listed below in order from best to worst:
 *
 *    - Native mode: best resolution supported by monitor
 *    - Standard timing: one of the standard resolutions supported by monitor
 *    - Established: one of the standard resolutions by VESA
 *    - "Safe": a resolution that generally all monitors support
 *    - Legacy VGA: VGA Mode 0x13 (320x200x8)
 *
 */
int video_early_bestMode(VideoMode *mode);



#endif