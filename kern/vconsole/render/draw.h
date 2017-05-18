/*
** File: kern/vconsole/render/draw.h
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors:
**
** Private header file
**
** Function prototypes for drawing a line in a VCon instance
**
*/

#ifndef _KERN_VCONSOLE_RENDER_DRAW_PH
#define _KERN_VCONSOLE_RENDER_DRAW_PH

#include "VConDrawFunc.h"

int vcon_vga_draw(VConCtrl *ctrl, VConChar *start, VConLine line, uint16_t row);

int vcon_graphics_draw(VConCtrl *ctrl, VConChar *start, VConLine line, uint16_t row);


#endif
