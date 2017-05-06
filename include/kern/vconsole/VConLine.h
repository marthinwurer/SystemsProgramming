#ifndef _KERN_VCONSOLE_VCONLINE_H
#define _KERN_VCONSOLE_VCONLINE_H

typedef struct VConLine_s VConLine;

#include <stdint.h>

#define VCON_LINE_FLAG_MASK     0x3
#define VCON_LINE_FLAG_DIRTY    0x1
#define VCON_LINE_FLAG_SCROLL   0x2
#define VCON_LINE_LENGTH_MASK   0xFFFC
#define VCON_LINE_LENGTH_LOC    0x2

/**
 * @brief Structure for an entry in the Line Information Table.
 *
 * An entry in the line table consists of two 16-bit words, length and offset.
 * The length word contains a 14-bit length of the line in columns and a 2 bit
 * flags field. The offset is a 16-bit index to the character table that
 * determines the location of the line in the table. The layout of the struct
 * is shown below:
 *
 * | 31              18 | 17 16 | 15                  0 |
 * +--------------------+-------+-----------------------+
 * | length             | flags |        offset         |
 * +--------------------+-------+-----------------------+
 * length (uint16_t)            offset (uint16_t)
 *
 * Offset (Bits 0-15)
 * --
 * The offset is an index in the character table that is the start of the
 * line.
 *
 * Flags (Bits 16-17)
 * --
 * The flags field is located in the lower two bits of the length member. This
 * field provides information for the controller. The following flags are
 * defined:
 *     Bit 16: dirty bit, if set this line should be redrawn
 *     Bit 17: scroll bit
 *
 * Length (Bits 18-31)
 * --
 * The length field is an unsigned 14-bit integer that determines the length
 * of the line in columns. If this length is less than the amount of columns
 * per row in the console, then the remaining columns on the line are assumed
 * to be VCON_NULLCELL (whitespace).
 *
 */
struct VConLine_s {

	uint16_t offset;
	uint16_t length;

} __attribute__(__packed__());



#endif