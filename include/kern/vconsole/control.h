#ifndef _KERN_VCONSOLE_CONTROL_H
#define _KERN_VCONSOLE_CONTROL_H

#include <kern/vconsole/err.h>
#include <kern/vconsole/VConCtrl.h>

/**
 * @brief Switches the currently displayed virtual console with another one
 */
int vcon_switch(VConCtrl *ctrl, VCon *console);

/**
 * @brief Toggles visibility and updates position of the software cursor.
 *
 * If a hardware cursor is available, this function does nothing. 
 */
int vcon_blink(VConCtrl *ctrl);


/**
 * @brief Writes the current virtual console buffer to the screen
 *
 * For VCON_MODE_VGATEXT, the buffer is copied to memory location 0xB8000
 * For VCON_MODE_GRAPHICS, the buffer is drawn using the associated paint
 * context.
 */
int vcon_redraw(VConCtrl *ctrl);


#endif