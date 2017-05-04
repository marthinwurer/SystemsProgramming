#ifndef _KERN_GRAPHICS_RECT_H
#define _KERN_GRAPHICS_RECT_H

#include <kern/graphics/Point.h>
#include <kern/graphics/Size.h>

typedef struct Rect_s {
	Point loc;
	Size size;
} Rect;

#define RGL_RECT_ZERO { .loc = POINT_ZERO, .size = SIZE_ZERO }

#define RGL_RECT_LEFT(rect) (rect.loc.x)
#define RGL_RECT_TOP(rect) (rect.loc.y)
#define RGL_RECT_RIGHT(rect) (rect.loc.x + rect.size.width)
#define RGL_RECT_BOTTOM(rect) (rect.loc.y + rect.size.height)

#endif