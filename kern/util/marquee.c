/*
** File: kern/util/marquee.c
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors:
**
** Implementations for marquee functions
**
*/

#include <kern/util/marquee.h>
#include <baseline/c_io.h>
#include <string.h>

static char marqueeStr[MARQUEE_STRING_MAX];
static unsigned marqueeStrLen = 0;
static unsigned marqueePos = 0;

static unsigned marqueeRow = 0;
static unsigned marqueeCol = 0;
static unsigned marqueeWidth = 0;

static void __clearRow(void);


int marquee_setText(const char *str) {

	memset(marqueeStr, ' ', MARQUEE_STRING_MAX);

	marqueeStrLen = 0;
	for (int i = 0; i != MARQUEE_STRING_MAX; ++i) {
		char c = str[i];
		if (c == '\0') {
			break;
		}

		marqueeStr[i] = str[i];
		++marqueeStrLen;
		
	}

	return 0;
}

int marquee_setRegion(unsigned col, unsigned row, unsigned width) {
	
	marqueeCol = col;
	marqueeRow = row;
	marqueeWidth = width;

	__clearRow();
	
	return 0;
}

int marquee_animate(void) {

	char buf[81];

	unsigned end = marqueeStrLen;
	if (marqueeStrLen < marqueeWidth) {
		end = marqueeWidth;
	}

	for (unsigned i = 0; i != marqueeWidth; ++i) {
		unsigned start = marqueePos + i;
		if (start >= end) {
			start -= end;
		}
		buf[i] = marqueeStr[start];
		//c_putchar_at(marqueeCol + i, marqueeRow, marqueeStr[start]);
	}
	buf[marqueeWidth] = '\0'; // terminate buffer

	c_puts_at(marqueeCol, marqueeRow, buf);

	// unsigned lastCol = marqueePos + marqueeWidth;
	// if (marqueePos == marqueeCol) {
	// 	lastCol = marqueeCol + marqueeWidth;
	// }
	// lastCol--;
	// c_putchar_at(lastCol, marqueeRow, ' ');
	if (++marqueePos == end) {
		marqueePos = 0;
	}


	return 0;
}

static void __clearRow(void) {
	for (unsigned i = 0; i != marqueeWidth; ++i) {
		c_putchar_at(marqueeCol + i, marqueeRow, ' ');
	}
}

// int marquee_animate(void) {

// 	for (unsigned i = 0; i != marqueeStrLen; ++i) {
// 		unsigned x = marqueePos + i + marqueeCol;
// 		if (x >= marqueeWidth) {
// 			x -= marqueeWidth;
// 		}
// 		c_putchar_at(x, marqueeRow, marqueeStr[i]);
// 	}
// 	unsigned lastCol = marqueePos;
// 	if (marqueePos == marqueeCol) {
// 		lastCol = marqueeCol + marqueeWidth;
// 	}
// 	lastCol--;
// 	c_putchar_at(lastCol, marqueeRow, ' ');
// 	if (++marqueePos == marqueeCol + marqueeWidth) {
// 		marqueePos = marqueeCol;
// 	}


// 	return 0;
// }
