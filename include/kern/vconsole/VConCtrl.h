#ifndef _KERN_VCONSOLE_VCONCTRL_H
#define _KERN_VCONSOLE_VCONCTRL_H

typedef struct VConCtrl_s VConCtrl;


#include <kern/vconsole/VCon.h>
#include <kern/vconsole/VConMode.h>
#include <kern/graphics/PaintContext.h>

//#include <kern/graphics/text/PSFont.h>

//#include <kern/video/fb/VideoFb.h>

/**
 * @brief Virtual console controller structure
 */
struct VConCtrl_s {
	VCon *current;
	VConMode mode;
	PaintContext ctx;
	// // font
	// PSFont font;
	// // palette
	// // framebuffer
	// VideoFb *fb;
};



#endif