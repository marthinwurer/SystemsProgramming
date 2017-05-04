#ifndef _KERN_VCONSOLE_VCONCTRL_H
#define _KERN_VCONSOLE_VCONCTRL_H

typedef struct VConCtrl_s VConCtrl;


#include <kern/vconsole/VCon.h>
#include <kern/vconsole/VConMode.h>
#include <kern/graphics/PaintContext.h>

/**
 * @brief Virtual console controller structure
 */
struct VConCtrl_s {
	VCon *current;
	VConMode mode;
	PaintContext ctx;
	uint8_t dirty;
};



#endif