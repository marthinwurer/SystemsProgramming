/*
** File: include/kern/vesa/VBEModeInfo.h
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors: 
**
** Typedef for the VBEModeInfo union. This type is a union of
** VBE2ModeInfo and VBE3ModeInfo structs
*/

#ifndef _KERN_VESA_VBEMODEINFO_H
#define _KERN_VESA_VBEMODEINFO_H


//
// VESA Mode attributes
//
#define VBE_MODEATTR_SUPPORTED    0x1
#define VBE_MODEATTR_RESERVED1    0x2
#define VBE_MODEATTR_TTY_SUPPORT  0x4
#define VBE_MODEATTR_COLOR        0x8
#define VBE_MODEATTR_GRAPHICS     0x10
#define VBE_MODEATTR_VGA          0x20
#define VBE_MODEATTR_WINDOWED     0x40
#define VBE_MODEATTR_FRAMEBUFFER  0x80
#define VBE_MODEATTR_DOUBLESCAN   0x100
#define VBE_MODEATTR_INTERLACED   0x200
#define VBE_MODEATTR_TRIPLEBUFFER 0x400
#define VBE_MODEATTR_STEREOSCOPIC 0x800
#define VBE_MODEATTR_DUALDISPLAY  0x1000
#define VBE_MODEATTR_RESERVED2    0x2000
#define VBE_MODEATTR_RESERVED3    0x4000
#define VBE_MODEATTR_RESERVED4    0x8000

//
// VESA Memory Models
//
#define VBE_MODEL_TEXT     0x0
#define VBE_MODEL_CGA      0x1
#define VBE_MODEL_HERCULES 0x2
#define VBE_MODEL_PLANAR   0x3
#define VBE_MODEL_PACKED   0x4
#define VBE_MODEL_NONCHAIN 0x5
#define VBE_MODEL_DIRECT   0x6
#define VBE_MODEL_YUV      0x7



#include <kern/vesa/VBE2ModeInfo.h>
#include <kern/vesa/VBE3ModeInfo.h>

typedef union VBEModeInfo_u {
	VBE2ModeInfo v2;
	VBE3ModeInfo v3;
} VBEModeInfo;


#endif