#ifndef _KERN_VESA_EDID_H
#define _KERN_VESA_EDID_H

#include <kern/vesa/EDIDRecord.h>

/**
 * @brief Gets the EDID record of the current connected monitor.
 */
int edid_getRecord(EDIDRecord *record);




#endif