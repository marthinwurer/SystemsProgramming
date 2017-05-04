#ifndef _KERN_VCONSOLE_VCONBUF_H
#define _KERN_VCONSOLE_VCONBUF_H

typedef struct VConBuf_s VConBuf;


#define VCON_LINE_FLAG_MASK     0x3
#define VCON_LINE_FLAG_DIRTY    0x1
#define VCON_LINE_FLAG_RESERVED 0x2
#define VCON_LINE_LENGTH_MASK   0xFFFC
#define VCON_LINE_LENGTH_LOC    0x2


/**
 * @brief Structure representing a virtual console buffer
 *
 * The VConBuf struct consists of two pointers, the lineTable and the charTable.
 *
 * lineTable (Line Information Table)
 * ----------------------------------
 * The lineTable is an array of 16-bit words, with each word being a line (row)
 * in the console buffer. It is required that the table has enough space for
 * all rows for a virtual console. An entry in this table is setup as followed:
 *
 * 15                               2       0
 * +--------------------------------+-------+
 * | Line length                    | Flags |
 * +--------------------------------+-------+
 *
 * Flags (Bits 0-1)
 * --
 * The first 2 bits contain the flags. The flags provide information to the
 * console controller. The following flags are defined:
 *   Bit 0: Dirty bit, if set the line will need to be redrawn to framebuffer
 *   Bit 1: Reserved
 *
 * Line length (Bits 2-15)
 * --
 * Bits 2-15 contain the length (in columns) of the line. The length is an
 * unsigned integer that determines how many characters in the charTable are
 * set for this row. If the length is less than the maximum columns for a
 * console buffer, then the rest of the columns on this line are assumed to be
 * null cells (whitespace).
 *
 * charTable (Character Cell Table)
 * --------------------------------
 * The charTable is an array of type VConChar, with each VConChar being a cell
 * in the virtual console. The length of this table must be the number of rows
 * multiplied by the number of columns for a virtual console.
 */
struct VConBuf_s {
	uint16_t *lineTable;
	VConChar *charTable;
};



#endif