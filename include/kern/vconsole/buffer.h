#ifndef _KERN_VCONSOLE_BUFFER_H
#define _KERN_VCONSOLE_BUFFER_H

#include <kern/vconsole/VConBuf.h>

#include <stdint.h>

#define vcon_buf_markDirty(buf, line) buf.lineTable[line] |= VCON_LINE_FLAG_DIRTY

int vcon_buf_init(VConBuf *buf);

int vcon_buf_clearLineTable(VConBuf *buf, uint16_t from, uint16_t to);

int vcon_buf_alloc(VConBuf *buf, void **pages);

int vcon_buf_lineLen(VConBuf *buf, uint16_t line);


#endif