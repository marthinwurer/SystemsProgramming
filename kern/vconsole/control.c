
#include <kern/vconsole/control.h>
#include <baseline/startup.h>

#include <string.h>

int vcon_switch(VConCtrl *ctrl, VCon *console) {
	ctrl->current = console;
	vcon_redraw(ctrl);

	return E_VCON_SUCCESS;
}


int vcon_redraw(VConCtrl *ctrl) {

	switch (ctrl->mode) {
		case VCON_MODE_VGATEXT:
			// copy the current console buffer to 0xB8000
			memcpy((void*)0xB8000, ctrl->current->buf, 80 * 25 * 2);
			// update the on screen cursor
			uint16_t index = (ctrl->current->cursorY * ctrl->current->columns) + ctrl->current->cursorX;
			__outb(0x3D4, 0xE);
			__outb(0x3D5, (index >> 8) & 0xFF);
			__outb(0x3D4, 0xF);
			__outb(0x3D5, index & 0xFF);
			break;
		case VCON_MODE_GRAPHICS:
			// draw all characters in the buffer using the paint context
			break;
	}


	return E_VCON_SUCCESS;
}