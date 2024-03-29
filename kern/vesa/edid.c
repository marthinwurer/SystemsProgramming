/*
** File: kern/vesa/edid.c
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors:
**
** Functions for getting the EDID, as well as parsing it.
**
*/

#include <kern/early/realmode.h>
#include <kern/vesa/vbe.h>
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

	regs16_t regs = {0};
	regs.eax = 0x4F15;
	regs.ebx = 0x01;
	regs.es = 0;
	regs.edi = VBE_BLOCK_ADDRESS;
	int32(0x10, &regs);

	int errorcode = E_VESA_ERROR;
	
	if ((regs.eax >> 16) == 0 && (regs.eax & 0xFF) == 0x4F) {
		memcpy(record, (void*)(regs.es * 0x10 + regs.edi), sizeof(EDIDRecord));
		errorcode = E_VESA_SUCCESS;
	}


	return errorcode;

}

int edid_getNativeTiming(EDIDRecord *record, VideoTiming *timing) {
	// if EDID v1.3 then the native timing is StandardTiming[0]
	if ((record->Version == 1 && record->Revision == 3) ||
	     record->FeatureSupport & 1) {
		return edid_parseDetailed(record, timing, 0);
	}

	return E_VESA_NOTIMING;
}

int edid_parseStandard(EDIDRecord *record, VideoTiming *timing, unsigned num) {

	if (num >= EDID_STANDARD_TIMING_COUNT) {
		return E_VESA_NOTIMING;
	}

	uint8_t byte0 = record->StandardTimings[num][0];
	uint8_t byte1 = record->StandardTimings[num][1];
	

	if (byte0 == 1 && byte0 == 1) {
		return E_VESA_NOTIMING;
	}

	uint16_t xres = (byte0 + 31) * 8;
	unsigned h, v;
	switch ((byte1 >> 6) & 3) {
		case EDID_RATIO_16_10: // 16:10
			h = 16;
			v = 10;
			break;
		case EDID_RATIO_4_3: // 4:3
			h = 4;
			v = 3;
			break;
		case EDID_RATIO_5_4: // 5:4
			h = 5;
			v = 4;
			break;
		case EDID_RATIO_16_9: // 16:9
			h = 16;
			v = 9;
			break;
	}

	uint16_t yres = (xres / h) * v;

	timing->width = xres;
	timing->height = yres;
	timing->refresh = (byte1 & 63) + 60;

	return E_VESA_SUCCESS;
}

int edid_parseDetailed(EDIDRecord *record, VideoTiming *timing, unsigned num) {

	if (num >= EDID_DETAILED_TIMING_COUNT) {
		return E_VESA_NOTIMING;
	}

	uint8_t *entry = record->DetailedTimings[num];

	// if the first three bytes and the 5th byte are zero, this block is a
	// monitor descriptor, not a timing
	if (entry[0] == 0 && entry[1] == 0 && entry[2] == 0 && entry[4] == 0) {
		return E_VESA_NOTIMING;
	}

	uint16_t pixelClock = (entry[1] << 8) | entry[0]; // LSB first

	uint16_t hActive = entry[2];
	hActive |= ((entry[4] & 0xF0) << 4);

	uint16_t hBlanking = entry[3];
	hBlanking |= (entry[4] & 0xF) << 8;

	uint16_t vActive = entry[5];
	vActive |= ((entry[7] & 0xF0) << 4);

	uint16_t vBlanking = entry[6];
	vBlanking |= (entry[7] & 0xF) << 8;

	timing->width = hActive;
	timing->height = vActive;
	timing->refresh = pixelClock / ((hActive + hBlanking) * (vActive + vBlanking));

	return E_VESA_SUCCESS;
}

