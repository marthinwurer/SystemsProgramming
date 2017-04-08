#ifndef _KERN_VGA_H
#define _KERN_VGA_H

//
// PORTS
//

// Read + write

#define VGA_PORT_CRTC_ADDR 0x3B4 // CRT Controller address register
#define VGA_PORT_CRTC_DATA 0x3B5 // CRT Controller Data register
#define VGA_PORT_ATTR      0x3C0 // Attribute address/data register
#define VGA_PORT_ATTR_DATA 0x3C1 // Attribute data read register
#define VGA_PORT_SEQ_ADDR  0x3C4 // Sequencer address register
#define VGA_PORT_SEQ_DATA  0x3C5 // Sequencer data register


// Write only

#define VGA_PORT_FCR     0x3BA // Feature control register
#define VGA_PORT_MISC_IN 0x3C2 // misc output register

// Read only

#define VGA_PORT_IS0 0x3C2 // Input Status #0 register
#define VGA_PORT_IS1 0x3BA // Input Status #1 register


//
// REGISTER INDICES
//


// Graphics Registers (GREG)

#define VGA_GREG_SR   0x0
#define VGA_GREG_ESR  0x1
#define VGA_GREG_CC   0x2
#define VGA_GREG_DR   0x3
#define VGA_GREG_RMS  0x4
#define VGA_GREG_GM   0x5
#define VGA_GREG_M    0x6
#define VGA_GREG_CDC  0x7
#define VGA_GREG_BM   0x8


#define VGA_REG_SET_RESET        VGA_REG_SR
#define VGA_REG_ENABLE_SET_RESET VGA_REG_ESR
#define VGA_REG_COLOR_COMPARE    VGA_REG_CC
#define VGA_REG_DATA_ROTATE      VGA_REG_DR
#define VGA_REG_READ_MAP_SELECT  VGA_REG_RMS
#define VGA_REG_GRAPHICS_MODE    VGA_REG_GM
#define VGA_REG_MISC_GRAPHICS    VGA_REG_M
#define VGA_REG_COLOR_DONT_CARE  VGA_REG_CDC
#define VGA_REG_BIT_MASK         VGA_REG_BM

// Sequence Registers (SREG)

// Attribute Controller Registers (ACREG)

// CRT Controller Registers (CRTREG)

// Color Registers (CREG)

// External Registers (EREG)

#endif