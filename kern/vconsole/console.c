
#include <kern/vconsole/console.h>


int vcon_clear(VCon *con) {

	if (con == NULL) {
		return E_VCON_ARGNULL;
	}

	uint32_t cellCount = con->columns * con->rows;
	VConChar *cellPtr = con->buf;

	while (cellCount != 0) {
		*cellPtr++ = (VConChar){
			.color = 0,
			.character = ' '
		};
		--cellCount;
	}

	return E_VCON_SUCCESS;

}