/*
** File: include/kern/util/marquee.h
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors: 
**
** Function prototypes for the marquee (horizontal scrolling text)
*/

#ifndef _KERN_DEBUG_MARQUEE_H
#define _KERN_DEBUG_MARQUEE_H

#define MARQUEE_STRING_MAX 255

int marquee_setText(const char *str);

int marquee_setRegion(unsigned col, unsigned row, unsigned width);

int marquee_animate(void);



#endif