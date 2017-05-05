#include <kern/vconsole/buffer.h>
#include <stddef.h>


int vcon_buf_init(VConBuf *buf) {
	buf->lineTable = NULL;
	buf->charTable = NULL;
	return E_VCON_SUCCESS;
}

int vcon_buf_clearLineTable(VConBuf *buf, uint16_t from, uint16_t to) {
	if (buf == NULL) {
		return E_VCON_ARGNULL;
	}

	for (uint16_t i = from; i != to; ++i) {
		// set the line to length = 0, flags = DIRTY
		buf->lineTable[i] = VCON_LINE_FLAG_DIRTY;
	}


	return E_VCON_SUCCESS;
}