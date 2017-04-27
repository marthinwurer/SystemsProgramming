
#include <kern/early/realmode.h>
#include <kern/vesa/edid.h>
#include <kern/vesa/err.h>
#include <string.h>

const VideoTiming const ESTABLISHED_TIMINGS1[] = {
	{ .width = 800, .height = 600, .refresh = 60 },
	{ .width = 800, .height = 600, .refresh = 56 },
	{ .width = 640, .height = 480, .refresh = 75 },
	{ .width = 640, .height = 480, .refresh = 72 },
	{ .width = 640, .height = 480, .refresh = 67 },
	{ .width = 640, .height = 480, .refresh = 60 },
	{ .width = 720, .height = 400, .refresh = 88 },
	{ .width = 720, .height = 400, .refresh = 70 }
};

const VideoTiming const ESTABLISHED_TIMINGS2[] = {
	{ .width = 1280, .height = 1024, .refresh = 75 },
	{ .width = 1024, .height = 768, .refresh = 75 },
	{ .width = 1024, .height = 768, .refresh = 70 },
	{ .width = 1024, .height = 768, .refresh = 60 },
	{ .width = 1024, .height = 768, .refresh = 87 },
	{ .width = 832, .height = 624, .refresh = 75 },
	{ .width = 800, .height = 600, .refresh = 75 },
	{ .width = 800, .height = 600, .refresh = 72 }
};


int edid_getRecord(EDIDRecord *record) {

/*
mov ax, 0x4f15
mov bl, 0x01
xor cx, cx
xor dx, dx
int 0x10
*/

	regs16_t regs;
	regs.eax = 0x4F15;
	regs.ebx = 0x01;
	regs.ecx = 0x0;
	regs.edx = 0x0;
	int32(0x10, &regs);

	int errorcode = E_VESA_ERROR;
	
	if ((regs.eax >> 16) == 0 && (regs.eax & 0xFF) == 0x4F) {
		memcpy(record, (void*)(regs.es * 0x10 + regs.edi), sizeof(EDIDRecord));
		errorcode = E_VESA_SUCCESS;
	}


	return errorcode;

}