#ifndef _KERN_VCONSOLE_CONTROL_H
#define _KERN_VCONSOLE_CONTROL_H

#include <kern/vconsole/err.h>

/**
 * @brief Switches the currently displayed virtual console with another one
 */
int vcon_switch(VConCtrl *ctrl, VCon *console);

/**
 * @brief Writes the current virtual console buffer to the screen
 *
 * For VCON_MODE_VGATEXT, the buffer is copied to memory location 0xB8000
 * For VCON_MODE_GRAPHICS, the buffer is drawn using the associated paint
 * context.
 */
int vcon_redraw(VConCtrl *ctrl);


#endif