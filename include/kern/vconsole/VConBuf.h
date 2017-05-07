#ifndef _KERN_VCONSOLE_VCONBUF_H
#define _KERN_VCONSOLE_VCONBUF_H

typedef struct VConBuf_s VConBuf;

#include <kern/vconsole/VConChar.h>
#include <kern/vconsole/VConLine.h>


/**
 * @brief Structure representing a virtual console buffer
 *
 * The VConBuf struct consists of two pointers, the lineTable and the charTable.
 *
 * lineTable (Line Information Table)
 * ----------------------------------
 * The lineTable is an array of type VConLine (or a 32-bit word). This table is
 * indexed by the rows/lines of the console. Therefore, it is required that the
 * table has enough space for all rows in the console.
 *
 * charTable (Character Cell Table)
 * --------------------------------
 * The charTable is an array of type VConChar, with each VConChar being a cell
 * in the virtual console. The length of this table must be the number of rows
 * multiplied by the number of columns for a virtual console.
 */
struct VConBuf_s {
	VConLine *lineTable;
	VConChar *charTable;
	uint32_t lineTableSize; // size of the line table in bytes
	uint32_t charTableSize; // size of the character table in bytes
};



#endif