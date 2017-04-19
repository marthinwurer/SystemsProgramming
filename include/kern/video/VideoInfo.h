#ifndef _KERN_VIDEO_VIDEOINFO_H
#define _KERN_VIDEO_VIDEOINFO_H


typedef struct VideoInfo_s {
	uint16_t vbeVersion;
	uint16_t softwareRev;
	uint16_t videoMemory;
	uint32_t capabilities;
	uint16_t modes[40];
	char oem[128];
	char vendor[128];
	char productName[128];
	char productRev[128];
} VideoInfo;


#endif