/*
** File: include/kern/video/fb/fb.h
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors: 
**
** Contains functions for writing to the linear framebuffer
*/

#ifndef _KERN_VIDEO_FB_FB_H
#define _KERN_VIDEO_FB_FB_H

#include <kern/video/fb/VideoFb.h>
#include <kern/video/color/VideoCol.h>


/**
 * @brief Initialize the framebuffer instance.
 *
 * Initializes all private members in the given instance and checks
 * if the framebuffer is valid. If successful, the framebuffer memory
 * will be cleared with the color black. This is not necessary, since
 * VBE normally clears the framebuffer when changing modes, but does
 * not hurt. 
 *
 * @return int
 *     E_SUCCESS Framebuffer initialized successfully
 *     E_ARGNULL fb argument was NULL
 */
int fb_init(VideoFb *fb);


/**
 * @brief Clear the framebuffer with the specified color. 
 * 
 * All pixels in the given framebuffer will be set to the given
 * color. Always returns E_SUCCESS.
 *
 * @return int
 *     E_SUCCESS Operation completed successfully
 *     E_ARGNULL fb argument was NULL
 * @param fb Pointer to configured framebuffer instance to clear
 * @param color The pixel color to set
 */
int fb_clear(VideoFb *fb, VideoCol color);


int fb_clearcolsf(VideoFb *fb, uint32_t offset, uint16_t cols, VideoCol color);


/**
 * @brief Calculates a pixel offset in a framebuffer at x and y.
 *
 * Calculates the offset in the framebuffer for a pixel at coordinates
 * x and y. The calculated offset will be set in the offset pointer. This
 * offset is the number of bytes from the framebuffer location and cannot
 * be used as an index. The x and y coordinates should be within the bounds
 * of the framebuffer's resolution, otherwise an offset that is outside the
 * bounds of the linear framebuffer array will be returned. Note that the
 * bounds are not checked due to performance reasons.
 *
 * @return int
 *     E_SUCCESS: The offset was calculated and stored in the offset pointer
 *     E_ARGNULL: fb or offset was set to NULL
 * @param fb Pointer to framebuffer to calculate an offset from struct's
 *           location member (@see VideoFb)
 * @param offset Pointer to store the calculated offset in.
 * @param x The X coordinate, should be 0 <= x < fb->width
 * @param y The Y coordinate, should be 0 <= y < fb->height
 */
int fb_offset(VideoFb *fb, uint32_t *offset, uint16_t x, uint16_t y);


/**
 * @brief Sets a pixel in the framebuffer at an x and y coordinate.
 *
 * @return int
 *     EVIDEO_SUCCESS Operation completed successfully
 *     EVIDEO_ARGNULL fb was set to NULL
 * @param fb Pointer to framebuffer to set pixel
 * @param x The x coordinate to set a pixel at (0 <= x < fb->width)
 * @param y The y coordinate to set a pixel at (0 <= y < fb->height)
 * @param color The color of the pixel to set
 */
int fb_putpixel(VideoFb *fb, uint16_t x, uint16_t y, VideoCol color);

/**
 * @brief Sets a number of columns in a row in the framebuffer.
 *
 * 
 */
int fb_putcols(VideoFb *fb, uint16_t x, uint16_t y, uint16_t bufsize, VideoCol buf[]);

int fb_putrow(VideoFb *fb, uint16_t row, VideoCol buf[]);

//
// Faster version of fb_putpixel. Instead of an x and y coordinate, uses
// a pre-calculated offset to set the pixel at. When using this function it
// is expected that you calculate an offset with fb_offset once, and increment
// it using fb_offsetIncr for each call to any of the fb_put* functions.
//
int fb_putpixelf(VideoFb *fb, uint32_t offset, VideoCol color);

int fb_putcolsf(VideoFb *fb, uint32_t offset, uint16_t bufsize, VideoCol buf[]);

int fb_putrowf(VideoFb *fb, uint32_t offset, VideoCol buf[]);



#endif