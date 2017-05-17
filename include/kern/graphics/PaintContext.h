/*
** File: include/kern/graphics/PaintContext.h
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors: 
**
** Typedef for the PaintContext struct
*/


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