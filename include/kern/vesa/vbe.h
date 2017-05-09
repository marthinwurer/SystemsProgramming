#ifndef _KERN_VESA_VBE_H
#define _KERN_VESA_VBE_H

#include <kern/vesa/VBEInfo.h>
#include <kern/vesa/VBEModeInfo.h>
#include <kern/vesa/err.h>

#include <stdint.h>

//
// VBE Return codes
//
#define VBE_RETURN_FUNCTION_SUPPORTED  0x004F
#define VBE_RETURN_FUNCTION_SUCCESS    0x0000
#define VBE_RETURN_FUNCTION_FAILED     0x0100
#define VBE_RETURN_FUNCTION_BAD_CONFIG 0x0200
#define VBE_RETURN_FUNCTION_INVALID    0x0300

//
// VBE Mode number masks
//
#define VBE_MODE_NUMBER     0x1FF      // bits 0-8
#define VBE_MODE_RESERVED   0x3E00     // bits 9-13
#define VBE_MODE_FLAG_LFB   0x4000     // bit 14
#define VBE_MODE_FLAG_CLEAR 0x8000     // bit 15

// physical address of where to store VBEInfo and VBEModeInfo structs
#define VBE_BLOCK_ADDRESS 0x3200

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


// Utility macro for calculating a physical address from a 
// vbeFarPointer (real mode segment and offset pair)
#define vbe_ptr(ptr) (((ptr >> 16) * 0x10) + (ptr & 0xFFFF))


// The list of video modes returned by vbe_getInfo is terminated
// with this constant
#define VBE_MODELIST_TERMINATOR 0xFFFF

//
// Initializes the VBE module. Currently unused.
//
// Returns:
//   E_VESA_SUCCESS
//
int vbe_init(void);

//
// Get the current VBE mode. If successful, the mode number is stored in
// modeVar. The result of the VBE function (register %ax) is set in vbeResult
// if vbeResult is not NULL.
// Returns:
//   E_VESA_SUCCESS: operation completed successfully
//   E_VESA_ERROR:   operation failed (%ax != 0x4f)
//   E_VESA_ARGNULL: modeVar was NULL
//
int vbe_currentMode(uint16_t *modeVar, uint16_t *vbeResult);

//
// Performs the VBE function Return VBE Controller Information. If successful,
// a shallow copy of the VBEInfo block is copied to the info argument. The
// result of the VBE function (register %ax) is set in vbeResult if vbeResult
// is not NULL.
//
// Note:
// Some VBE implementations return pointers that point within the structure.
// Calling this function again will overwrite the data pointed by said pointers.
// Be sure to process info before calling vbe_getInfo or vbe_getModeInfo again
// to avoid data corruption.
//
// Returns:
//   E_VESA_SUCCESS: vbe operation completed successfully, VBEInfo block copied
//                to info pointer
//   E_VESA_ERROR:   vbe operation failed, info pointer unchanged
//   E_VESA_ARGNULL: info was NULL
//
int vbe_getInfo(VBEInfo *info, uint16_t *vbeResult);

//
// Performs the VBE function Return VBE Mode Information. If successful, the
// VBEModeInfo block is copied to the modeInfo argument. The result of the VBE
// function (register %ax) is set in vbeResult if vbeResult is not NULL.
//
// Returns:
//   E_VESA_SUCCESS: operation completed successfully, VBEModeInfo block copied
//                to modeInfo pointer
//   E_VESA_ERROR:   operation failed, modeInfo pointer unchanged
//   E_VESA_ARGNULL: modeInfo was NULL
//
int vbe_getModeInfo(uint16_t mode, VBEModeInfo *modeInfo, uint16_t *vbeResult);

//
// Performs the VBE function Set VBE Mode. Initializes the controller and sets
// a VBE mode from the mode argument.
//
// Returns:
//    E_VESA_SUCCESS: operation completed successfully
//    E_VESA_ERROR: operation failed, mode might be unsupported by display, etc
//
int vbe_setMode(uint16_t mode, uint16_t *vbeResult);

#endif