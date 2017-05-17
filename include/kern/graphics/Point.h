/*
** File: include/kern/graphics/Point.h
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors: 
**
** Typedef for the Point primitive
*/

#ifndef _KERN_GRAPHICS_POINT_H
#define _KERN_GRAPHICS_POINT_H


typedef struct Point_s {
	int x;
	int y;
} Point;

#define POINT_ZERO ((Point){.x = 0, .y = 0})


#endif