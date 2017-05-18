/*
** File: kern/vconsole/render/VConDrawFunc.h
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors:
**
** Private Header
** Typedef for a VConDrawFunc function pointer type
**
*/

#ifndef _KERN_VCONSOLE_RENDER_VCONDRAWFUNC_PH
#define _KERN_VCONSOLE_RENDER_VCONDRAWFUNC_PH



#include <kern/vconsole/VConCtrl.h>
#include <kern/vconsole/VConChar.h>
#include <kern/vconsole/VConLine.h>

#include <stdint.h>


typedef int (*VConDrawFunc)(VConCtrl*, VConChar*, VConLine, uint16_t);


#endif
