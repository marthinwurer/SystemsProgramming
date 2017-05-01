#ifndef _KERN_GRAPHICS_TEXT_TEXT_H
#define _KERN_GRAPHICS_TEXT_TEXT_H

#include <kern/graphics/PaintContext.h>
#include <kern/graphics/Point.h>

int graphics_drawString(PaintContext *ctx, Point *p, const char *string);

int graphics_drawChar(PaintContext *ctx, Point *p, char c);



#endif