
#include <kern/vconsole/console.h>
#include <kern/vconsole/control.h>
#include <kern/vconsole/buffer.h>
#include <stddef.h>



int vcon_init(VCon *con, uint16_t rows, uint16_t columns) {
	if (con == NULL) {
		return E_VCON_ARGNULL;
	}

	con->rows = rows;
	con->columns = columns;
	con->cursorX = 0;
	con->cursorY = 0;
	con->scrollStart = 0;
	con->scrollEnd = rows;
	con->buf = (VConBuf){
		.lineTable = NULL,
		.charTable = NULL
	};
	con->controller = NULL;


	return E_VCON_SUCCESS;
}

int vcon_clear(VCon *con) {

	if (con == NULL) {
		return E_VCON_ARGNULL;
	}

	vcon_buf_clearLineTable(con, 0, con->rows);

	con->cursorX = 0;
	con->cursorY = con->scrollStart;

	return E_VCON_SUCCESS;

}

int vcon_clearScroll(VCon *con) {
	if (con == NULL) {
		return E_VCON_ARGNULL;
	}

	vcon_buf_clearLineTable(con, con->scrollStart, con->scrollEnd);

	con->cursorX = 0;
	con->cursorY = 0;

	return E_VCON_SUCCESS;
}

int vcon_putchar(VCon *con, char ch) {
	if (con == NULL) {
		return E_VCON_ARGNULL;
	}

	if (con->cursorY == con->scrollEnd) {
		vcon_scroll(con, 1);
		--con->cursorY;
	}

	VConChar cell = VCON_NULLCELL;
	int newrow = 0;  // 1 to move cursor to next row
	uint16_t curRow = con->cursorY;
	uint16_t curCol = con->cursorX;
	VConLine curLine = con->buf.lineTable[curRow];
	// get the index in the charTable that the cursor is pointing to
	// get the offset in the lineTable and add the current column.
	unsigned index = curLine.offset + curCol;
	

	switch (ch) {
		case '\n':
			// newlines, set the line length = curCol, set the cursor to
			// the next line
			con->cursorX = 0;
			curLine.length = curCol << VCON_LINE_LENGTH_LOC;
			newrow = 1;
			break;
		case '\r':
			// carriage return, set the x position of the cursor to 0
			// the line length shouldn't be touched
			con->cursorX = 0;
			break;
		case '\t':
			// tabs, add null cells until the cursor is at the next indentation
			// level or we reach the maximum columns. Update cursor.
			break;
		default:
			cell.character = ch;
			con->buf.charTable[index] = cell;

			// update the line length, if cursorX is greater than or equal
			// to the length, set the length to cursorX + 1
			if (curCol >= curLine.length >> VCON_LINE_LENGTH_LOC) {
				// set the line length to col + 1
				curLine.length = (curCol + 1) << VCON_LINE_LENGTH_LOC;
			}
			if (++con->cursorX == con->columns) {
				con->cursorX = 0;
				newrow = 1;
			}

			break;
	}

	if (newrow) {
		++con->cursorY;
	}

	// mark line as dirty
	curLine.length |= VCON_LINE_FLAG_DIRTY;
	// store line in lineTable
	con->buf.lineTable[curRow] = curLine;

	return E_VCON_SUCCESS;
}

int vcon_putcharAt(VCon *con, char ch, uint16_t x, uint16_t y) {
	if (con == NULL) {
		return E_VCON_ARGNULL;
	}
	if (x >= con->columns || y >= con->rows) {
		return E_VCON_BOUNDS;
	}

	VConLine curLine = con->buf.lineTable[y];

	int index = curLine.offset + x;

	if (ch == '\n') {
		curLine.length = x << VCON_LINE_LENGTH_LOC;
	} else {
		con->buf.charTable[index] = (VConChar){
			.character = ch,
			.color = VCON_DEFAULT_COLOR
		};
		if (x >= curLine.length >> VCON_LINE_LENGTH_LOC) {
			curLine.length = (x + 1) << VCON_LINE_LENGTH_LOC;
		}
	}

	curLine.length |= VCON_LINE_FLAG_DIRTY;
	con->buf.lineTable[y] = curLine;


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
	
	// uint16_t rows = con->scrollMaxY - con->scrollMinY;
	// uint16_t cols = con->scrollMaxX - con->scrollMinX;

	// if (lines > con->scrollMaxY - con->scrollMinY) {
	// 	vcon_clearScroll(con);
	// 	con->cursorX = con->scrollMinX;
	// 	con->cursorY = con->scrollMinY;
	// } else {

	// 	uint16_t line = con->scrollMinY;
	// 	uint16_t lineEnd = con->scrollMaxY - lines;
	// 	VConChar *from = con->buf + ((line + lines) * con->columns) + con->scrollMinX;
	// 	VConChar *to = con->buf + (line * con->columns) + con->scrollMinX;
		
	// 	uint16_t colsToNextLine = con->columns - con->scrollMaxX + con->scrollMinX;

	// 	for (; line != lineEnd; ++line) {
	// 		for (unsigned i = 0; i <= cols; ++i) {
	// 			*to++ = *from++;
	// 		}
	// 		to += colsToNextLine;
	// 		from += colsToNextLine;
	// 	}

	// 	for (; line != con->scrollMaxY; ++line) {
	// 		for (unsigned i = 0; i != cols; ++i) {
	// 			*to++ = NULL_CELL;
	// 		}
	// 		to += colsToNextLine;
	// 	}

	// }
	

	return E_VCON_SUCCESS;
}

int vcon_setCursor(VCon *con, uint16_t x, uint16_t y) {
	if (con == NULL) {
		return E_VCON_ARGNULL;
	}

	if (x >= con->columns || y >= con->rows) {
		return E_VCON_BOUNDS;
	}

	con->cursorX = x;
	con->cursorY = y;

	return E_VCON_SUCCESS;
}

int vcon_setScroll(VCon *con, uint16_t start, uint16_t end) {
	if (con == NULL) {
		return E_VCON_ARGNULL;
	}
	
	if (start >= end || end > con->rows) {
		return E_VCON_BOUNDS;
	}

	con->scrollStart = start;
	con->scrollEnd = end;
	con->cursorX = 0;
	con->cursorY = start;

	return E_VCON_SUCCESS;
}

