
#include <kern/vconsole/console.h>
#include <kern/vconsole/control.h>
#include <kern/vconsole/buffer.h>
#include <stddef.h>

#define DEFAULT_COLOR 7

#define NULL_CELL (VConChar){ .color = DEFAULT_COLOR, .character = ' ' }

#define calcIndex(con, x, y) ((con->columns * y) + x)

static void __markDirty(VCon *con);

int vcon_init(VCon *con, uint16_t rows, uint16_t columns) {
	if (con == NULL || buf == NULL) {
		return E_VCON_ARGNULL;
	}

	con->rows = rows;
	con->columns = columns;
	con->cursorX = 0;
	con->cursorY = 0;
	con->scrollMinX = 0;
	con->scrollMinY = 0;
	con->scrollMaxX = columns;
	con->scrollMaxY = rows;
	con->buf = (VConBuf){
		.lineTable = NULL,
		.charTable = NULL
	}
	con->controller = NULL;


	return E_VCON_SUCCESS;
}

int vcon_clear(VCon *con) {

	if (con == NULL) {
		return E_VCON_ARGNULL;
	}

	vcon_buf_clearLineTable(&con->buf, 0, con->rows);

	con->cursorX = con->scrollMinX;
	con->cursorY = con->scrollMinY;

	// uint32_t cellCount = con->columns * con->rows;
	// VConChar *cellPtr = con->buf;

	// while (cellCount != 0) {
	// 	*cellPtr++ = NULL_CELL;
	// 	--cellCount;
	// }

	//__redraw(con);

	return E_VCON_SUCCESS;

}

int vcon_clearScroll(VCon *con) {
	if (con == NULL) {
		return E_VCON_ARGNULL;
	}

	unsigned scrollMinY = con->scrollMinY;
	unsigned scrollMaxY = con->scrollMaxY;
	unsigned columns = con->columns;
	unsigned scrollCols = con->scrollMaxX - con->scrollMinX;

	if (scrollCols == columns) {
		// scroll area spans all columns
		// we can just mark the lines as empty (length = 0, dirty = 1)
		vcon_buf_clearLineTable(&con->buf, scrollMinY, scrollMaxY);
	} else {

		// set all cells in the scroll area to VCON_NULLCELL
		// mark all lines in scroll area as dirty

		unsigned index = scrollMinY * columns + con->scrollMinX;

		for (unsigned r = scrollMinY; r != scrollMaxY; ++r) {
			for (unsigned c = 0; c != scrollCols; ++c) {
				con->buf.charTable[index + c] = VCON_NULLCELL;
			}

			// update the line length in the line table
			vcon_buf_lineLen(&con->buf, r);

			// mark the line as dirty
			vcon_buf_markDirty(con.buf, r);

			index += columns;
		}
	}

	con->cursorX = 0;
	con->cursorY = 0;


	// if (con->scrollMinX >= con->scrollMaxX || con->scrollMinY >= con->scrollMaxY) {
	// 	return E_VCON_BADSCROLL;
	// }

	// uint16_t cols = con->scrollMaxX - con->scrollMinX;
	// unsigned index = con->scrollMinY * con->columns;
	
	// for (int r = con->scrollMinY; r != con->scrollMaxY; ++r) {
	// 	for (int c = 0; c != cols; c++) {
	// 		con->buf[index + c] = NULL_CELL;
	// 	}
	// 	index += con->columns; // next line
	// }

	//__redrawCells(con, calcIndex(con, con->scrollMinX, con->scrollMaxX),
	//                   (con->scrollMaxX - con->scrollMinX) * (con->scrollMaxY - con->scrollMinY));


	return E_VCON_SUCCESS;
}

int vcon_putchar(VCon *con, char ch) {
	if (con == NULL) {
		return E_VCON_ARGNULL;
	}

	VConChar cell = NULL_CELL;
	int newrow = 0;  // 1 to move cursor to next row
	int index = calcIndex(con, con->cursorX, con->cursorY);
	unsigned cellsWritten = 0;

	switch (ch) {
		case '\n':
			// for (unsigned i = 0; i != con->scrollMaxX - con->cursorX; ++i) {
			// 	con->buf[index] = cell;
			// 	++cellsWritten;
			// }
			con->cursorX = con->scrollMinX;
			newrow = 1;
			break;
		case '\r':
			con->cursorX = con->scrollMinX;
			break;
		case '\t':
			// todo
			break;
		default:
			cell.character = ch;
			con->buf[index] = cell;
			++cellsWritten;
			if (++con->cursorX == con->scrollMaxX) {
				con->cursorX = con->scrollMinX;
				newrow = 1;
			}

			break;
	}

	if (newrow) {
		if (++con->cursorY == con->scrollMaxY) {
			vcon_scroll(con, 1);
			--con->cursorY;
		}
	}

	//__redrawCells(con, index, cellsWritten);
	__markDirty(con);

	return E_VCON_SUCCESS;
}

int vcon_putcharAt(VCon *con, char ch, uint16_t x, uint16_t y) {
	if (con == NULL) {
		return E_VCON_ARGNULL;
	}
	if (x >= con->columns || y >= con->rows) {
		return E_VCON_BOUNDS;
	}

	int index = calcIndex(con, x, y);
	unsigned cells = 0;

	if (ch == '\n') {
		unsigned limit;
		if (x > con->scrollMaxX) {
			limit = con->columns;
		} else if (x >= con->scrollMinX) {
			limit = con->scrollMaxX;
		} else {
			limit = con->scrollMinX - 1;
		}

		while (x <= limit) {
			con->buf[index + cells] = NULL_CELL;
			++x;
			++cells;
		}


	} else {
		con->buf[index] = (VConChar){
			.character = ch,
			.color = DEFAULT_COLOR
		};

		cells = 1;

	}

	//__redrawCells(con, index, cells);
	__markDirty(con);

	return E_VCON_SUCCESS;
}

int vcon_puts(VCon *con, const char *str) {
	if (con == NULL || str == NULL) {
		return E_VCON_ARGNULL;
	}

	char ch;
	while ((ch = *str++) != '\0') {
		vcon_putchar(con, ch);
	}

	return E_VCON_SUCCESS;
}

int vcon_putsAt(VCon *con, const char *str, uint16_t x, uint16_t y) {
	if (con == NULL) {
		return E_VCON_ARGNULL;
	}
	
	char ch;
	int error = E_VCON_SUCCESS;
	while ((ch = *str++) != '\0') {
		error = vcon_putcharAt(con, ch, x, y);
		if (++x == con->columns || error == E_VCON_BOUNDS) {
			break;
		}
	}

	return error;
}

int vcon_scroll(VCon *con, uint16_t lines) {
	if (con == NULL) {
		return E_VCON_ARGNULL;
	}
	
	uint16_t rows = con->scrollMaxY - con->scrollMinY;
	uint16_t cols = con->scrollMaxX - con->scrollMinX;

	if (lines > con->scrollMaxY - con->scrollMinY) {
		vcon_clearScroll(con);
		con->cursorX = con->scrollMinX;
		con->cursorY = con->scrollMinY;
	} else {

		uint16_t line = con->scrollMinY;
		uint16_t lineEnd = con->scrollMaxY - lines;
		VConChar *from = con->buf + ((line + lines) * con->columns) + con->scrollMinX;
		VConChar *to = con->buf + (line * con->columns) + con->scrollMinX;
		
		uint16_t colsToNextLine = con->columns - con->scrollMaxX + con->scrollMinX;

		for (; line != lineEnd; ++line) {
			for (unsigned i = 0; i <= cols; ++i) {
				*to++ = *from++;
			}
			to += colsToNextLine;
			from += colsToNextLine;
		}

		for (; line != con->scrollMaxY; ++line) {
			for (unsigned i = 0; i != cols; ++i) {
				*to++ = NULL_CELL;
			}
			to += colsToNextLine;
		}

		// uint16_t index = calcIndex(con, con->scrollMinX, con->scrollMinY);
		// for (unsigned i = 0; i != rows; ++i) {
		// 	__redrawCells(con, index, cols);
		// 	index += con->columns;
		// }

		


		// VConChar *tempLines[lines];
		// for (unsigned i = 0; i != lines; ++i) {
		// 	tempLines[i] = con->buf.lineTable[i];
		// }

		// uint16_t lineCount = con->scrollMaxY - con->scrollMinY - lines;
		// uint16_t nextIndex;
		// for (unsigned i = 0; i ! lineCount; ++i) {
		// 	nextIndex = i + lines;
		// 	con->buf.lineTable[i] = con->buf.lineTable[nextIndex];
		// }

		// for (unsigned i = 0; i != lines; ++i) {
		// 	con->buf.lineTable[lineCount + i] = tempLines[i];
		// }

		// uint16_t line;
		// uint16_t lineCount = con->scrollMaxY - lines;
		// uint16_t nchars = con->scrollMaxX - con->scrollMinX + 1;
		// VConChar *from, *to;

		// for (line = con->scrollMinY; line != lineCount; ++line) {
		// 	from = con->buf + (((line + lines) * con->columns) + con->scrollMinX);
		// 	to = con->buf + ((line * con->columns) + con->scrollMinX);
		// 	for (uint16_t c = 0; c != nchars; ++c) {
		// 		*to++ = *from++;
		// 	}
		// }

		// for (; line != con->scrollMaxY; ++line) {
		// 	to = con->buf + ((line * con->columns) + con->scrollMinX);
		// 	for (uint16_t c = 0; c != nchars; ++c) {
		// 		*to++ = NULL_CELL;
		// 	}
		// }

	}

	//__redraw(con);
	__markDirty(con);
	

	return E_VCON_SUCCESS;
}

int vcon_setCursor(VCon *con, uint16_t x, uint16_t y) {
	if (con == NULL) {
		return E_VCON_ARGNULL;
	}

	if (x < con->scrollMinX || y < con->scrollMinY || 
	    x >= con->scrollMaxX || y >= con->scrollMaxY) {
		return E_VCON_BOUNDS;
	}

	con->cursorX = x;
	con->cursorY = y;

	return E_VCON_SUCCESS;
}

int vcon_setScroll(VCon *con, uint16_t minX, uint16_t minY, uint16_t maxX, uint16_t maxY) {
	if (con == NULL) {
		return E_VCON_ARGNULL;
	}
	
	if (minX >= maxX || minY >= maxY || maxX > con->columns || maxY > con->rows) {
		return E_VCON_BOUNDS;
	}

	con->scrollMinX = minX;
	con->scrollMaxX = maxX;
	con->scrollMinY = minY;
	con->scrollMaxY = maxY;
	con->cursorX = minX;
	con->cursorY = minY;

	return E_VCON_SUCCESS;
}


void __markDirty(VCon *con) {
	if (con->controller != NULL) {
		con->controller->dirty = 1;
	}
}
