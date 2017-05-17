/*
** File: include/kern/vesa/EDIDRecord.h
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors: 
**
** Typedef for the EDIDRecord struct
*/

#ifndef _KERN_VESA_EDIDRECORD_H
#define _KERN_VESA_EDIDRECORD_H

#define EDID_STANDARD_TIMING_COUNT 8
#define EDID_DETAILED_TIMING_COUNT 4

// Aspect ratios
#define EDID_RATIO_16_10 0         // 16:10
#define EDID_RATIO_4_3   1         // 4:3
#define EDID_RATIO_5_4   2         // 5:4
#define EDID_RATIO_16_9  3         // 16:9


struct EDIDRecord_s {

	uint8_t Header[8];         // Should be 00 FF FF FF FF FF FF 00
	
	// Vendor / Product Identification
	uint16_t ManufactureId;
	uint16_t ProductCode;
	uint32_t SerialCode;
	uint8_t ManufactureWeek;
	uint8_t ManufactureYear;

	// EDID Structure Version / Revision

	uint8_t Version;
	uint8_t Revision;
	
	// Basic Display Parameters / Features

	uint8_t VideoInputDefinition;
	uint8_t MaxHorizontalImageSize;
	uint8_t MaxVerticalImageSize;
	uint8_t Gamma;
	uint8_t FeatureSupport;

	// Color Characteristics

	uint8_t RedGreenLowBits;
	uint8_t BlueWhiteLowBits;
	uint8_t RedX;
	uint8_t RedY;
	uint8_t GreenX;
	uint8_t GreenY;
	uint8_t BlueX;
	uint8_t BlueY;
	uint8_t WhiteX;
	uint8_t WhiteY;

	// Timings

	uint8_t EstbTimings1;
	uint8_t EstbTimings2;
	uint8_t ManufacturerTimings;

	// Standard Timing Identification

	uint8_t StandardTimings[EDID_STANDARD_TIMING_COUNT][2];

	// Detailed Timing Descriptions

	uint8_t DetailedTimings[EDID_DETAILED_TIMING_COUNT][18];

	uint8_t ExtensionFlag;
	uint8_t Checksum;



} __attribute__((__packed__));


typedef struct EDIDRecord_s EDIDRecord;


#endif