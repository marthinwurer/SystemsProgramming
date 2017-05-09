#ifndef _KERN_VCONSOLE_VCONCHAR_H
#define _KERN_VCONSOLE_VCONCHAR_H

#include <stdint.h>

typedef struct VConChar_s {
	char character;
	uint8_t color;
} VConChar;


#endif