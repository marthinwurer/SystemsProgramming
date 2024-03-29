/*
** File: include/kern/graphics/shapes/rect.h
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors: 
**
** Function prototypes for drawing/filling a rectangle
*/

#ifndef _KERN_GRAPHICS_SHAPES_RECT_H
#define _KERN_GRAPHICS_SHAPES_RECT_H

#include <kern/graphics/PaintContext.h>
#include <kern/graphics/Rect.h>


int graphics_drawRect(PaintContext *ctx, Rect *rect);

int graphics_fillRect(PaintContext *ctx, Rect *rect);


#endif