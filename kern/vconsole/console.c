
#include <kern/vconsole/console.h>
#include <stddef.h>

#define NULL_CELL (VConChar){ .color = 7, .character = ' ' }

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

int vcon_putcharAt(VCon *con, char ch, uint16_t row, uint16_t col) {
	if (con == NULL) {
		return E_VCON_ARGNULL;
	}
	(void)ch; (void)row; (void)col;

	return E_VCON_SUCCESS;
}

int vcon_puts(VCon *con, const char *str) {
	if (con == NULL) {
		return E_VCON_ARGNULL;
	}
	(void)str;

	return E_VCON_SUCCESS;
}

int vcon_putsAt(VCon *con, const char *str, uint16_t row, uint16_t col) {
	if (con == NULL) {
		return E_VCON_ARGNULL;
	}
	(void)str; (void)row; (void)col;

	return E_VCON_SUCCESS;
}

int vcon_scroll(VCon *con, uint16_t lines) {
	if (con == NULL) {
		return E_VCON_ARGNULL;
	}
	(void)lines;

	return E_VCON_SUCCESS;
}

int vcon_setCursor(VCon *con, uint16_t row, uint16_t column) {
	if (con == NULL) {
		return E_VCON_ARGNULL;
	}
	(void)row; (void)column;

	return E_VCON_SUCCESS;
}

int vcon_setScroll(VCon *con, uint16_t minX, uint16_t minY, uint16_t maxX, uint16_t maxY) {
	if (con == NULL) {
		return E_VCON_ARGNULL;
	}
	(void)minX; (void)minY; (void)maxX; (void)maxY;

	return E_VCON_SUCCESS;
}
