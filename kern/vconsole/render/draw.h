#ifndef _KERN_VCONSOLE_RENDER_DRAW_PH
#define _KERN_VCONSOLE_RENDER_DRAW_PH

#include "VConDrawFunc.h"

int vga_draw(VCon *con, VConChar *start, VConLine line, uint16_t row);

int graphics_draw(VCon *con, VConChar *start, VConLine line, uint16_t row);


#endif