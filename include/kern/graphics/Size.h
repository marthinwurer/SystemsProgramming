#ifndef _KERN_GRAPHICS_SIZE_H
#define _KERN_GRAPHICS_SIZE_H

typedef struct Size_s {
	int width;
	int height;
} Size;

#define SIZE_ZERO ((Size){ .width = 0, .height = 0 })

#endif