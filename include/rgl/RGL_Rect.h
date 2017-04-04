#ifndef _RGL_RGL_RECT_H
#define _RGL_RGL_RECT_H

#include "RGL_Point.h"
#include "RGL_Size.h"

typedef struct RGL_Rect_s {
	RGL_Point loc;
	RGL_Size size;
} RGL_Rect;

#define RGL_RECT_ZERO { .loc = RGL_POINT_ZERO, .size = RGL_SIZE_ZERO }

#define RGL_RECT_LEFT(rect) (rect.loc.x)
#define RGL_RECT_TOP(rect) (rect.loc.y)
#define RGL_RECT_RIGHT(rect) (rect.loc.x + rect.size.width)
#define RGL_RECT_BOTTOM(rect) (rect.loc.y + rect.size.height)

#endif