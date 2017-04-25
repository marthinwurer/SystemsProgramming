
#include <kern/early/realmode.h>
#include <kern/vesa/edid.h>
#include <string.h>

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

	int errorcode = 1;
	
	if ((regs.eax >> 16) == 0 && (regs.eax & 0xFF) == 0x4F) {
		memcpy(record, (void*)(regs.es * 0x10 + regs.edi), sizeof(EDIDRecord));
		errorcode = 0;
	}


	return errorcode;

}