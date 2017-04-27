#ifndef _KERN_VIDEO_VIDEOINFO_H
#define _KERN_VIDEO_VIDEOINFO_H

#include <stdint.h>
#include <kern/video/VideoStatus.h>

#define VIDEO_INFO_SIZE 512


struct VideoInfo_s {
	VideoStatus status;
	uint16_t vbeVersion;            // VBE version, usually 0x300 (VBE 3.0)
	uint16_t softwareRev;           // Software revision
	uint16_t videoMemory;           // Video memory in 64 KB blocks
	uint32_t capabilities;          // bitfield for video card capabilities
	uint32_t modeCount;             // number of available VBE modes
	uint16_t *modes;                // Ptr in buf to VBE mode list
	char *oem;                      // string containing the oem
	char *vendor;
	char *productName;
	char *productRev;
};

typedef union VideoInfo_u {
	struct VideoInfo_s info;
	uint8_t buf[VIDEO_INFO_SIZE];
} VideoInfo;


#endif