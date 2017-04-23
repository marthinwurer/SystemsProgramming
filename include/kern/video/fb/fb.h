#ifndef _KERN_VIDEO_FB_FB_H
#define _KERN_VIDEO_FB_FB_H

#include <kern/video/fb/VideoFb.h>
#include <kern/video/color/VideoCol.h>


int fb_init(VideoFb *fb);


//
// Clear the framebuffer with the specified color. All pixels in the
// framebuffer will be set the given color.
//
int fb_clear(VideoFb *fb, VideoCol color);

int fb_setPixel(VideoFb *fb, uint16_t x, uint16_t y, VideoCol color);

int fb_setColumns(VideoFb *fb, uint16_t row, uint16_t col, uint16_t bufsize, VideoCol buf[]);

int fb_setRow(VideoFb *fb, uint16_t row, VideoCol buf[]);

#endif