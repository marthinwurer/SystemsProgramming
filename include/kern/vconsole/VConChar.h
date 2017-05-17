#ifndef _KERN_VCONSOLE_VCONCHAR_H
#define _KERN_VCONSOLE_VCONCHAR_H

#include <stdint.h>

// light-gray on black
#define VCON_DEFAULT_COLOR 0x7

#define VCON_NULLCELL ((VConChar){ .character = ' ', .color = VCON_DEFAULT_COLOR })


/**
 * @brief Structure representing a cell in a virtual console buffer.
 *
 * The VConChar struct contains two bytes, the color byte and the character
 * byte. Note that this structure is similar to an entry in a VGA text-mode
 * buffer.
 *
 * color (Color byte)
 * ------------------
 * The color member contains color information for the cell. The upper 4 bits
 * determine the background color and the lower 4 bits determine the foreground
 * color. For VGA text-mode consoles, the background may only be 3 bits, since
 * the most significant bit is typically used for blinking.
 */
typedef struct VConChar_s {
	char character;
	uint8_t color;
} VConChar;


#endif