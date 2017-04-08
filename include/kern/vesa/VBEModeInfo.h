#ifndef _KERN_VESA_VBEMODEINFO_H
#define _KERN_VESA_VBEMODEINFO_H

//
// VESA BIOS Extensions Mode information block structure
//
struct VBEModeInfo_s {
	uint16_t attributes;
	uint8_t windowA;
	uint8_t windowB;
	uint16_t granularity;
	uint16_t windowSize;
	uint16_t segmentA;
	uint16_t segmentB;
	uint32_t winFuncPtr;
	uint16_t pitch;
	uint16_t width;
	uint16_t height;
	uint8_t wchar;
	uint8_t ychar;
	uint8_t planes;
	uint8_t bpp;
	uint8_t banks;
	uint8_t memoryModel;
	uint8_t bankSize;
	uint8_t imagePages;
	uint8_t reserved0;
	uint8_t redMask;
	uint8_t redPosition;
	uint8_t blueMask;
	uint8_t bluePosition;
	uint8_t reservedMask;
	uint8_t reservedPosition;
	uint8_t directColorAttributes;
	uint32_t framebuffer;
	uint32_t offScreenMemOff;
	uint16_t offScreenMemSize;
	uint8_t reserved1[206];
} __attribute__((packed));

typedef struct VBEModeInfo_s VBEModeInfo;


#endif