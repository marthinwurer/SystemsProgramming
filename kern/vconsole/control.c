/*
** File: kern/vconsole/control.c
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors:
**
** Implementations for functions that act on a VConCtrl instance
**
*/

#include <kern/vconsole/control.h>
#include <baseline/startup.h>
#include <kern/graphics/text/text.h>

#include <string.h>

#include "render/VConDrawFunc.h"
#include "render/VConScrollFunc.h"
#include "render/draw.h"
#include "render/scroll.h"

static const VConDrawFunc DRAW_FUNC_TABLE[] = {
	vcon_vga_draw,
	vcon_graphics_draw
};

static const VConScrollFunc SCROLL_FUNC_TABLE[] = {
	vga_scroll,
	graphics_scroll
};

int vcon_switch(VConCtrl *ctrl, VCon *console) {
	ctrl->current = console;
	vcon_redraw(ctrl);

	return E_VCON_SUCCESS;
}


int vcon_redraw(VConCtrl *ctrl) {

	if (ctrl == NULL) {
		return E_VCON_ARGNULL;
	}

	VCon *con = ctrl->current;
	VConBuf buf = con->buf;
	VConMode mode = ctrl->mode;
	VConLine line;
	uint16_t rows = con->rows;
	//uint16_t cols = con->columns;
	//int inScroll = 0;

	VConDrawFunc drawFunc = DRAW_FUNC_TABLE[mode];
	//VConScrollFunc scrollFunc = SCROLL_FUNC_TABLE[mode];

	for (unsigned r = 0; r != rows; ++r) {
		line = buf.lineTable[r];

		if (line.length & VCON_LINE_FLAG_DIRTY) {
				// line is dirty, redraw it
				drawFunc(ctrl, buf.charTable + line.offset, line, r);
				buf.lineTable[r].length &= ~VCON_LINE_FLAG_DIRTY;
			}
		// if (inScroll) {

		// } else {
		
		// 	if (line.length & VCON_LINE_FLAG_SCROLL) {
		// 		inScroll = 1;
		// 	} else if (line.length & VCON_LINE_FLAG_DIRTY) {
		// 		// line is dirty, redraw it
		// 		drawFunc(con, buf.charTable + line.offset, line, r);
		// 	}
		// }
	}

	// unsigned cellHeight, cellWidth, cols, currentCol;
	// Point p;
	// unsigned cells;
	// VConBuf buf;
	// unsigned offset;
	// unsigned length;

	// switch (ctrl->mode) {
	// 	case VCON_MODE_VGATEXT:
	// 		buf = ctrl->current->buf;
	// 		offset = 0;
	// 		for (uint16_t r = 0, rows = ctrl->current->rows; r != rows; ++r) {
	// 			length = buf.lineTable[r].length >> VCON_LINE_LENGTH_LOC;
	// 			memcpy((void*)0xB8000 + offset,
	// 			        buf.charTable + buf.lineTable[r].offset,
	// 				    length * sizeof(VConChar));
	// 			memset((void*)0xB8000 + offset + (length * sizeof(VConChar)),
	// 			        0,
	// 					(ctrl->current->columns - length) * sizeof(VConChar));
				
	// 			offset += 160;
	// 		}
	// 		break;
	// 	case VCON_MODE_GRAPHICS:
	// 		// draw all characters in the buffer using the paint context
	// 		// cellHeight = ctrl->ctx.font.height;
	// 		// cellWidth = ctrl->ctx.font.width;
	// 		// cols = ctrl->current->columns;
	// 		// currentCol = 0;

	// 		// p = (Point){ .x = 0, .y = 0 };
	// 		// cells = ctrl->current->rows * ctrl->current->columns;
	// 		// buf = ctrl->current->buf;

	// 		// for (unsigned i = 0; i != cells; ++i) {
	// 		// 	graphics_drawChar(&ctrl->ctx, &p, buf[i].character);
	// 		// 	p.x += cellWidth;
	// 		// 	if (++currentCol == cols) {
	// 		// 		p.x = 0;
	// 		// 		p.y += cellHeight;
	// 		// 		currentCol = 0;
	// 		// 	}
	// 		// }
	// 		break;
	// }
	return E_VCON_SUCCESS;
}
