#ifndef _KERN_VCONSOLE_RENDER_DRAW_PH
#define _KERN_VCONSOLE_RENDER_DRAW_PH

#include "VConDrawFunc.h"

int vcon_vga_draw(VConCtrl *ctrl, VConChar *start, VConLine line, uint16_t row);

int vcon_graphics_draw(VConCtrl *ctrl, VConChar *start, VConLine line, uint16_t row);


#endif