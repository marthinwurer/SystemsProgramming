#ifndef _KERN_VESA_VBE_H
#define _KERN_VESA_VBE_H

#include <kern/vesa/VBEInfo.h>

#include <kern/baseline/bootstrap.h>

extern VBEInfo const *VESA_INFO;


int vesa_vbeGetInfo(void);




#endif