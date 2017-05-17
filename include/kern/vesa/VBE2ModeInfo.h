/*
** File: include/kern/vesa/VBE2ModeInfo.h
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors: 
**
** Typedef for VBE2ModeInfo struct (VBE version 2.0 Mode Info block)
*/

#ifndef _KERN_VESA_VBE2MODEINFO_H
#define _KERN_VESA_VBE2MODEINFO_H

//
// VESA BIOS 2.0 Extensions Mode information block structure
//
struct VBE2ModeInfo_s {
	// Mandatory information
	uint16_t ModeAttributes;
	uint8_t  WinAAttributes;
	uint8_t  WinBAttributes;
	uint16_t WinGranularity;
	uint16_t WinSize;
	uint16_t WinASegment;
	uint16_t WinBSegment;
	uint32_t WinFuncPtr;
	uint16_t BytesPerScanLine;
	// VBE 1.2 and above
	uint16_t XResolution;
	uint16_t YResolution;
	uint8_t  XCharSize;
	uint8_t  YCharSize;
	uint8_t  NumberOfPlanes;
	uint8_t  BitsPerPixel;
	uint8_t  NumberOfBanks;
	uint8_t  MemoryModel;
	uint8_t  BankSize;
	uint8_t  NumberOfImagePages;
	uint8_t  reserved0;
	uint8_t  RedMaskSize;
	uint8_t  RedFieldPosition;
	uint8_t  GreenMaskSize;
	uint8_t  GreenFieldPosition;
	uint8_t  BlueMaskSize;
	uint8_t  BlueFieldPosition;
	uint8_t  RvsdMaskSize;
	uint8_t  RvsdFieldPosition;
	uint8_t  DirectColorModeInfo;
	// VBE 2.0 and above
	uint32_t PhysBasePtr;
	uint32_t OffScreenMemOffset;
	uint16_t OffScreenMemSize;
	uint8_t reserved1[206];
} __attribute__((packed));

typedef struct VBE2ModeInfo_s VBE2ModeInfo;


#endif