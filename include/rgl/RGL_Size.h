#ifndef _RGL_RGLSIZE_H
#define _RGL_RGLSIZE_H

typedef struct RGL_Size_s {
	int width;
	int height;
} RGL_SIZE;

#define RGL_SIZE_ZERO { .width = 0, .height = 0 }

#endif