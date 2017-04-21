
#include <kern/vesa/vbe.h>
#include <kern/early/realmode.h>

#include <string.h>


static int __performVBEFunction(uint16_t function, regs16_t *regs);


// VBE2 signature
static const char VBE_SIG[] = { 'V', 'B', 'E', '2'};


int vbe_init(void) {
	// unused return success
	return VBE_SUCCESS;
}

int vbe_getInfo(VBEInfo *info, uint16_t *vbeResult) {

	if (info == NULL) {
		return VBE_ARGNULL;
	}

	VBEInfo *block = (VBEInfo*)VBE_BLOCK_ADDRESS;

	// copy signature to block struct
	memcpy(block->signature, VBE_SIG, sizeof(VBE_SIG));

	regs16_t regs;
	regs.es = 0;
	regs.edi = VBE_BLOCK_ADDRESS;

	int errorcode = VBE_ERROR;
	
	// perform VBE_FUNCTION_GETINFO
	if (__performVBEFunction(VBE_FUNCTION_GETINFO, &regs)) {
		// success
		// copy block structure to argument
		memcpy(info, block, sizeof(VBEInfo));
		errorcode = VBE_SUCCESS;
	}

	if (vbeResult != NULL) {
		*vbeResult = regs.eax;
	}

	return errorcode;

}

int vbe_getModeInfo(uint16_t mode, VBEModeInfo *modeInfo, uint16_t *vbeResult) {

	// Check for NULL pointers
	if (modeInfo == NULL) {
		return VBE_ARGNULL;
	}

	VBEModeInfo *block = (VBEModeInfo*)VBE_BLOCK_ADDRESS;

	regs16_t regs;
	regs.es = 0;
	regs.edi = VBE_BLOCK_ADDRESS;
	regs.ecx = mode;

	int errorcode = VBE_ERROR;

	if (__performVBEFunction(VBE_FUNCTION_GETMODEINFO, &regs)) {
		memcpy(modeInfo, block, sizeof(VBEModeInfo));
		errorcode = VBE_SUCCESS;
	}

	if (vbeResult != NULL) {
		*vbeResult = regs.eax;
	}


	return errorcode;
}

int vbe_setMode(uint16_t mode, uint16_t *vbeResult) {

	int errorcode = VBE_ERROR;

	regs16_t regs;
	regs.ebx = mode;

	if (__performVBEFunction(VBE_FUNCTION_SETMODE, &regs)) {
		errorcode = VBE_SUCCESS;
	}

	if (vbeResult != NULL) {
		*vbeResult = regs.eax;
	}


	return errorcode;
}

int vbe_currentMode(uint16_t *modeVar, uint16_t *vbeResult) {

	if (modeVar == NULL) {
		return VBE_ARGNULL;
	}

	int errorcode = VBE_ERROR;

	regs16_t regs;
	
	if (__performVBEFunction(VBE_FUNCTION_CURRENTMODE, &regs)) {
		*modeVar = regs.ebx;
		errorcode = VBE_SUCCESS;
	}

	if (vbeResult != NULL) {
		*vbeResult = regs.eax;
	}

	return errorcode;

}



// static helper functions

int __performVBEFunction(uint16_t function, regs16_t *regs) {
	regs->eax = VBE_AX | function;
	int32(0x10, regs);

	return regs->eax == (VBE_RETURN_FUNCTION_SUPPORTED | VBE_RETURN_FUNCTION_SUCCESS);
}
