#ifndef _KERN_VCONSOLE_RENDER_VCONDRAWFUNC_PH
#define _KERN_VCONSOLE_RENDER_VCONDRAWFUNC_PH



#include <kern/vconsole/VConCtrl.h>
#include <kern/vconsole/VConChar.h>
#include <kern/vconsole/VConLine.h>

#include <stdint.h>


typedef int (*VConDrawFunc)(VConCtrl*, VConChar*, VConLine, uint16_t);


#endif