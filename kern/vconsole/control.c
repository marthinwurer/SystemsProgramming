
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
	/*switch (ctrl->mode) {
		case VCON_MODE_VGATEXT:
			
			break;
		case VCON_MODE_GRAPHICS:
			// draw all characters in the buffer using the paint context
			__asm("nop");
			unsigned cellHeight = ctrl->ctx.font.height;
			unsigned cellWidth = ctrl->ctx.font.width;
			unsigned cols = ctrl->current->columns;
			unsigned currentCol = 0;

			Point p = (Point){ .x = 0, .y = 0 };
			unsigned cells = ctrl->current->rows * ctrl->current->columns;
			VConChar *buf = ctrl->current->buf;

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
	}*/


	return vcon_redrawCells(ctrl, 0, ctrl->current->rows * ctrl->current->columns);
}

int vcon_redrawCells(VConCtrl *ctrl, unsigned index, unsigned cells) {
	if (ctrl == NULL) {
		return E_VCON_ARGNULL;
	}


	unsigned cellHeight, cellWidth, cols, currentCol;
	Point p;
	VConChar *buf;

	switch (ctrl->mode) {
		case VCON_MODE_VGATEXT:
			memcpy((uint16_t*)0xB8000 + index, ctrl->current->buf + index, cells * sizeof(VConChar));
			index += cells;
			// __outb(0x3D4, 0xE);
			// __outb(0x3D5, (index >> 8) & 0xFF);
			// __outb(0x3D4, 0xF);
			// __outb(0x3D5, index & 0xFF);
			break;
		case VCON_MODE_GRAPHICS:
			// draw the specified cells in the buffer using the paint context
			cellHeight = ctrl->ctx.font.height;
			cellWidth = ctrl->ctx.font.width;
			cols = ctrl->current->columns;
			currentCol = index % ctrl->current->columns;

			p = (Point){ 
				.x = currentCol * cellWidth, 
				.y = (index / ctrl->current->columns) * cellHeight
			};
			buf = ctrl->current->buf;

			for (unsigned i = 0; i != cells; ++i) {
				graphics_drawChar(&ctrl->ctx, &p, buf[index + i].character);
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