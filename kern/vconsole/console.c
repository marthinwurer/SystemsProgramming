
#include <kern/vconsole/console.h>
#include <stddef.h>

#define DEFAULT_COLOR 7

#define NULL_CELL (VConChar){ .color = DEFAULT_COLOR, .character = ' ' }

#define calcIndex(con, x, y) ((con->columns * y) + x)

int vcon_init(VCon *con, VConChar *buf, uint16_t rows, uint16_t columns) {
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
	con->buf = buf;

	return E_VCON_SUCCESS;
}

int vcon_clear(VCon *con) {

	if (con == NULL) {
		return E_VCON_ARGNULL;
	}

	uint32_t cellCount = con->columns * con->rows;
	VConChar *cellPtr = con->buf;

	while (cellCount != 0) {
		*cellPtr++ = NULL_CELL;
		--cellCount;
	}

	return E_VCON_SUCCESS;

}

int vcon_clearScroll(VCon *con) {
	if (con == NULL) {
		return E_VCON_ARGNULL;
	}

	if (con->scrollMinX >= con->scrollMaxX || con->scrollMinY >= con->scrollMaxY) {
		return E_VCON_BADSCROLL;
	}

	uint16_t cols = con->scrollMaxX - con->scrollMinX;
	unsigned index = con->scrollMinY * con->columns;
	
	for (int r = con->scrollMinY; r != con->scrollMaxY; ++r) {
		for (int c = 0; c != cols; c++) {
			con->buf[index + c] = NULL_CELL;
		}
		index += con->columns; // next line
	}

	return E_VCON_SUCCESS;
}

int vcon_putchar(VCon *con, char ch) {
	if (con == NULL) {
		return E_VCON_ARGNULL;
	}

	VConChar cell = NULL_CELL;
	int newrow = 0;  // 1 to move cursor to next row
	int index = calcIndex(con, con->cursorX, con->cursorY);

	switch (ch) {
		case '\n':
			for (unsigned i = con->scrollMaxX - con->cursorX; i != 0; --i) {
				con->buf[index++] = cell;
			}
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
	con->buf[index] = (VConChar){
		.character = ch,
		.color = DEFAULT_COLOR
	};

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
	int error;
	while ((ch = *str++) != '\0') {
		error = vcon_putcharAt(con, ch, x, y);
		if (++x == con->columns || error == E_VCON_BOUNDS) {
			return E_VCON_BOUNDS;
		}
	}

	return E_VCON_SUCCESS;
}

int vcon_scroll(VCon *con, uint16_t lines) {
	if (con == NULL) {
		return E_VCON_ARGNULL;
	}
	
	if (lines > con->scrollMaxY - con->scrollMinY) {
		vcon_clearScroll(con);
		con->cursorX = con->scrollMinX;
		con->cursorY = con->scrollMinY;
	} else {

		uint16_t line;
		uint16_t lineCount = con->scrollMaxY - lines;
		uint16_t nchars = con->scrollMaxX - con->scrollMinX + 1;
		VConChar *from, *to;

		for (line = con->scrollMinY; line != lineCount; ++line) {
			from = con->buf + (((line + lines) * con->columns) + con->scrollMinX);
			to = con->buf + ((line * con->columns) + con->scrollMinX);
			for (uint16_t c = 0; c != nchars; ++c) {
				*to++ = *from++;
			}
		}

		for (; line != con->scrollMaxY; ++line) {
			to = con->buf + ((line * con->columns) + con->scrollMinX);
			for (uint16_t c = 0; c != nchars; ++c) {
				*to++ = NULL_CELL;
			}
		}

	}

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
	con->cursorY = maxY;

	return E_VCON_SUCCESS;
}
