#ifndef _KERN_VESA_VBE_H
#define _KERN_VESA_VBE_H

#include <kern/vesa/VBEInfo.h>
#include <kern/vesa/VBEModeInfo.h>

#include <stdint.h>

#define VBE_RETURN_FUNCTION_SUPPORTED 0x004F
#define VBE_RETURN_FUNCTION_SUCCESS 0x0000
#define VBE_RETURN_FUNCTION_FAILED 0x0100
#define VBE_RETURN_FUNCTION_BAD_CONFIG 0x0200
#define VBE_RETURN_FUNCTION_INVALID 0x0300

#define VBE_MODE_NUMBER 0x1FF          // bits 0-8
#define VBE_MODE_RESERVED 0x3E00       // bits 9-13
#define VBE_MODE_FLAG_LFB 0x4000       // bit 14
#define VBE_MODE_FLAG_CLEAR 0x8000     // bit 15

// vbe module uses physical address 0x500 when calling BIOS functions
#define VBE_BLOCK_ADDRESS 0x3000

// Constant stored in %ah when performing int 10h

#define VBE_AX 0x4F00

// VBE function codes (goes in %al)

#define VBE_FUNCTION_GETINFO              0x00
#define VBE_FUNCTION_GETMODEINFO          0x01
#define VBE_FUNCTION_SETMODE              0x02
#define VBE_FUNCTION_CURRENTMODE          0x03
#define VBE_FUNCTION_STATE                0x04
#define VBE_FUNCTION_DISPLAYWINDOWCONTROL 0x05
#define VBE_FUNCTION_SCANLINE             0x06
#define VBE_FUNCTION_DISPLAYSTART         0x07
#define VBE_FUNCTION_DACPALETTE           0x08
#define VBE_FUNCTION_PALETTE              0x09
#define VBE_FUNCTION_PROTECTEDMODE        0x0A

// error codes returned from vbe_* functions

#define VBE_ARGNULL -1
#define VBE_ERROR 1
#define VBE_SUCCESS 0

//
// Initializes the VBE module.
//
int vbe_init(void);

//
// Get the current VBE mode. If successful, the mode number is stored in
// modeVar.
//
int vbe_currentMode(uint16_t *modeVar, uint16_t *vbeResult);

int vbe_getInfo(VBEInfo *info, uint16_t *vbeResult);

int vbe_getModeInfo(uint16_t mode, VBEModeInfo *modeInfo, uint16_t *vbeResult);

int vbe_setMode(uint16_t mode, uint16_t *vbeResult);

#endif