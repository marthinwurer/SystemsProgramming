/*
** File: include/kern/graphics/text/PSFont.h
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors: 
**
** Typedef for the PC Screen Font format
*/

#ifndef _KERN_GRAPHICS_TEXT_PSFONT_H
#define _KERN_GRAPHICS_TEXT_PSFONT_H

#include <stdint.h>

#define PSFONT_MAGIC 0x864AB572

// PC Screen font
typedef struct PSFont_s {

	// header
	uint32_t magic;
	uint32_t version;
	uint32_t headerSize;
	uint32_t flags;
	uint32_t glyphs;
	uint32_t bytesPerGlyph;
	uint32_t height;
	uint32_t width;

	// glyph data
	uint8_t *glyphMap;


} PSFont;


#endif