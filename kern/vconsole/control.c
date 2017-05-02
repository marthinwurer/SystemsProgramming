
#include <kern/vconsole/control.h>


int vcon_switch(VConCtrl *ctrl, VCon *console) {
	ctrl->current = console;
	vcon_redraw(ctrl);
	return 0;
}


int vcon_redraw(VConCtrl *ctrl) {

	switch (ctrl->mode) {
		case VCON_MODE_VGATEXT:
			// copy the current console buffer to 0xB8000
			break;
		case VCON_MODE_GRAPHICS:
			// draw all characters in the buffer using the paint context
			break;
	}


	return 0;
}