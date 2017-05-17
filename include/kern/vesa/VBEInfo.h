/*
** File: include/kern/vesa/VBEInfo.h
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors: 
**
** Typedef for the VBEInfo struct
*/

#ifndef _KERN_VESA_VBEINFO_H
#define _KERN_VESA_VBEINFO_H

#include <stdint.h>

//
// VESA BIOS Extensions (VBE) Info structure
//
struct VBEInfo_s {
	char signature[4];       // must be set to "VESA"
	uint16_t version;        // VBE version, high byte = major, low byte = minor
	uint32_t oem;            // segment:offset pointer to OEM
	uint32_t capabilities;   // bitfield for video card capabilities
	uint32_t videoModes;     // segment:offset pointer to list of modes
	uint16_t videoMemory;    // amount of memory in 64KB blocks
	uint16_t softwareRev;    // software revision
	uint32_t vendor;         // segment:offset to card vendor string
	uint32_t productName;    // segment:offset to card model name string
	uint32_t productRev;     // segment:offset to product revision string
	char reserved[222];      // reserved data
	char oemData[256];       // OEM BIOSes store stuff here

} __attribute__((packed));

typedef struct VBEInfo_s VBEInfo;


#endif