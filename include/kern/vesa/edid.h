#ifndef _KERN_VESA_EDID_H
#define _KERN_VESA_EDID_H

#include <kern/vesa/EDIDRecord.h>

#include <kern/video/VideoTiming.h>

extern const VideoTiming const ESTABLISHED_TIMINGS1[8];

extern const VideoTiming const ESTABLISHED_TIMINGS2[8];

/**
 * @brief Gets the EDID record of the current connected monitor.
 */
int edid_getRecord(EDIDRecord *record);


/**
 * @brief Parses the EDIDRecord and tries to find the native timing.
 *
 * Reads the given EDIDRecord and stores (if found) the native or preferred
 * timing in the given timing pointer. If the native timing cannot be found,
 * EDID_NOTIMING is returned and the timing struct is not modified. Otherwise,
 * the timing struct is set accordingly and EDID_SUCCESS is returned.
 */
int edid_getNativeTiming(EDIDRecord *record, VideoTiming *timing);

/**
 * @brief Parse the EDIDRecord Standard Timings section.
 *
 * Reads the Standard Timings table for a given index and parses it
 * as a VideoTiming struct. If no timing exists at the given index, then
 * E_VESA_NOTIMING is returned.
 *
 */
int edid_parseStandard(EDIDRecord *record, VideoTiming *timing, unsigned num);

/**
 * @brief Parse the EDIDRecord Detailed Timings section.
 *
 * Reads the Detailed Timings table for a given index and parses it as a
 * VideoTiming struct. If no timing exists at the given index, then
 * E_VESA_NOTIMING is returned.
 */
int edid_parseDetailed(EDIDRecord *record, VideoTiming *timing, unsigned num);



#endif