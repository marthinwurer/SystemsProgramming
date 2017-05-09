#ifndef _KERN_VCONSOLE_VCONSOLE_H
#define _KERN_VCONSOLE_VCONSOLE_H

typedef struct VCon_s VCon;


#include <kern/vconsole/VConCtrl.h>
#include <kern/vconsole/VConChar.h>

#include <stdint.h>

#define VCON_DEFAULT_TABSIZE 4

// temporary maximums
// we only have room for a 128x48 (1024x768px) console buffer
// once this branch merges with master this maximum will go away
// since we can dynamically allocate memory as much as needed.

#define VCON_COLUMNS_MAX 128
#define VCON_ROWS_MAX 48

#define CONSOLE_BUF_ADDRESS 0x4C00


#define vcon_cursor(columns, row, col) (col + (row * columns))

struct VCon_s {

	uint16_t rows;
	uint16_t columns;
	uint16_t cursorX;
	uint16_t cursorY;
	uint16_t scrollMinX;
	uint16_t scrollMinY;
	uint16_t scrollMaxX;
	uint16_t scrollMaxY;
	uint8_t tabSize;
	VConChar *buf;
	VConCtrl *controller;
};


#endif