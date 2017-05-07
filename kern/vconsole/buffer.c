#include <kern/vconsole/buffer.h>

#include <stddef.h>


int vcon_buf_clearLineTable(VCon *con, uint16_t from, uint16_t to) {
	if (con == NULL) {
		return E_VCON_ARGNULL;
	}

	for (uint16_t i = from; i != to; ++i) {
		// set the line to length = 0, flags = DIRTY
		con->buf.lineTable[i].length = VCON_LINE_FLAG_DIRTY;
	}


	return E_VCON_SUCCESS;
}

int vcon_buf_initLineTable(VCon *con) {
	if (con == NULL) {
		return E_VCON_ARGNULL;
	}

	unsigned lineSize = con->columns * sizeof(VConChar);
	unsigned offset = 0;
	unsigned rows = con->rows;

	for (unsigned r = 0; r != rows; ++r) {
		con->buf.lineTable[r] = (VConLine){
			.offset = offset,
			.length = VCON_LINE_FLAG_DIRTY
		};
		offset += lineSize;
	}

	return E_VCON_SUCCESS;
}