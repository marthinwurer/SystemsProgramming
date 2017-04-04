#ifndef _KERN_VGA13_VGA13_H
#define _KERN_VGA13_VGA13_H

#include <stdint.h>

#define VGA13_WIDTH 320
#define VGA13_HEIGHT 200

#define VGA13_FRAMEBUFFER (uint8_t*)(0xA0000)

#define VGA13_offset(x, y) ((VGA13_WIDTH * y) + x)

int vga13_clear(uint8_t color);

int vga13_getColumns(unsigned row, unsigned col, unsigned bufsize, uint8_t buf[]);

int vga13_getPixel(unsigned x, unsigned y, uint8_t *pixelVar);

int vga13_getRow(unsigned row, uint8_t buf[]);

int vga13_init(void);

int vga13_setColumns(unsigned row, unsigned col, unsigned bufsize, uint8_t buf[]);

int vga13_setPixel(unsigned x, unsigned y, uint8_t color);

int vga13_setRow(unsigned row, uint8_t buf[]);


#endif