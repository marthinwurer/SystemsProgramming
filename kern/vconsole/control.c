
#include <kern/vconsole/control.h>
#include <baseline/startup.h>

#include <kern/graphics/text/text.h>


#include <string.h>

int vcon_switch(VConCtrl *ctrl, VCon *console) {
	ctrl->current = console;
	vcon_redraw(ctrl);

	return E_VCON_SUCCESS;
}


int vcon_redraw(VConCtrl *ctrl) {

	if (ctrl == NULL) {
		return E_VCON_ARGNULL;
	}

	unsigned cellHeight, cellWidth, cols, currentCol;
	Point p;
	unsigned cells;
	VConChar *buf;

	switch (ctrl->mode) {
		case VCON_MODE_VGATEXT:
			memcpy((void*)0xB8000, ctrl->current->buf, 80 * 25 * sizeof(VConChar));
			break;
		case VCON_MODE_GRAPHICS:
			// draw all characters in the buffer using the paint context
			cellHeight = ctrl->ctx.font.height;
			cellWidth = ctrl->ctx.font.width;
			cols = ctrl->current->columns;
			currentCol = 0;

			p = (Point){ .x = 0, .y = 0 };
			cells = ctrl->current->rows * ctrl->current->columns;
			buf = ctrl->current->buf;

			for (unsigned i = 0; i != cells; ++i) {
				graphics_drawChar(&ctrl->ctx, &p, buf[i].character);
				p.x += cellWidth;
				if (++currentCol == cols) {
					p.x = 0;
					p.y += cellHeight;
					currentCol = 0;
				}
			}
			break;
	}
	return E_VCON_SUCCESS;
}