/*
** File: include/kern/vconsole/buffer.h
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors: 
**
** Function prototypes for modifying a VConBuf
*/
#ifndef _KERN_VCONSOLE_BUFFER_H
#define _KERN_VCONSOLE_BUFFER_H

#include <kern/vconsole/VCon.h>
#include <kern/vconsole/err.h>

#include <stdint.h>

int vcon_buf_clearLineTable(VCon *con, uint16_t from, uint16_t to);

int vcon_buf_initLineTable(VCon *con);


#endif