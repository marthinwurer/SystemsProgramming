#ifndef _KERN_VCONSOLE_CONSOLE_H
#define _KERN_VCONSOLE_CONSOLE_H

#include <stdint.h>
#include <kern/vconsole/err.h>
#include <kern/vconsole/VCon.h>

int vcon_init(VCon *con, uint16_t rows, uint16_t columns);

int vcon_clear(VCon *con);

int vcon_clearScroll(VCon *con);

int vcon_putchar(VCon *con, char ch);

int vcon_putcharAt(VCon *con, char ch, uint16_t x, uint16_t y);

int vcon_puts(VCon *con, const char *str);

int vcon_putsAt(VCon *con, const char *str, uint16_t c, uint16_t y);

int vcon_scroll(VCon *con, uint16_t lines);

int vcon_setCursor(VCon *con, uint16_t x, uint16_t y);

int vcon_setScroll(VCon *con, uint16_t minX, uint16_t minY, uint16_t maxX, uint16_t maxY);



#endif