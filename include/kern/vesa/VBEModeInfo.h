#ifndef _KERN_VESA_VBEMODEINFO_H
#define _KERN_VESA_VBEMODEINFO_H

#include <kern/vesa/VBE2ModeInfo.h>
#include <kern/vesa/VBE3ModeInfo.h>

typedef union VBEModeInfo_u {
	VBE2ModeInfo v2;
	VBE3ModeInfo v3;
} VBEModeInfo;


#endif