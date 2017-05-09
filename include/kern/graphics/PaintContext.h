#ifndef _KERN_GRAPHICS_PAINTCONTEXT_H
#define _KERN_GRAPHICS_PAINTCONTEXT_H

#include <kern/video/color/VideoCol.h>
#include <kern/video/fb/VideoFb.h>
#include <kern/graphics/text/PSFont.h>


typedef struct PaintContext_s {

	VideoCol drawCol;
	VideoCol fillCol;
	PSFont font;
	VideoFb *fb;


} PaintContext;


#endif