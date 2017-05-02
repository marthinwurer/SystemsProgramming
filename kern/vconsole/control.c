
#include <kern/vconsole/control.h>

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
			break;
		case VCON_MODE_GRAPHICS:
			// draw all characters in the buffer using the paint context
			break;
	}


	return E_VCON_SUCCESS;
}