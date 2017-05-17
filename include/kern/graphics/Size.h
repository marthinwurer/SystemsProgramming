/*
** File: include/kern/graphics/Size.h
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors: 
**
** Typedef for the Size primitive
*/

#ifndef _KERN_GRAPHICS_SIZE_H
#define _KERN_GRAPHICS_SIZE_H

typedef struct Size_s {
	int width;
	int height;
} Size;

#define SIZE_ZERO ((Size){ .width = 0, .height = 0 })

#endif