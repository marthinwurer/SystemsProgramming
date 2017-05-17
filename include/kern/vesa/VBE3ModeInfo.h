/*
** File: include/kern/vesa/VBE3ModeInfo.h
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors: 
**
** Typedef for the VBE3ModeInfo struct (VBE version 3.0 Mode Info block)
*/

#ifndef _KERN_VESA_VBE3MODEINFO_H
#define _KERN_VESA_VBE3MODEINFO_H

//
// VESA BIOS 3.0 Extensions Mode information block structure
//
struct VBE3ModeInfo_s {
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
	uint32_t reserved1;
	uint16_t reserved2;
	// VBE 3.0 and above
	uint16_t LinBytesPerScanLine;
	uint8_t  BnkNumberOfImagePages;
	uint8_t  LinNumberOfImagePages;
	uint8_t  LinRedMaskSize;
	uint8_t  LinRedFieldPosition;
	uint8_t  LinGreenMaskSize;
	uint8_t  LinGreenFieldPosition;
	uint8_t  LinBlueMaskSize;
	uint8_t  LinBlueFieldPosition;
	uint8_t  LinRsvdMaskSize;
	uint8_t  LinRsvdFieldPosition;
	uint32_t MaxPixelClock;
	uint8_t reserved3[189];
} __attribute__((packed));

typedef struct VBE3ModeInfo_s VBE3ModeInfo;


#endif