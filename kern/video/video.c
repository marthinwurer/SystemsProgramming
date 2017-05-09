
#include <kern/vesa/vbe.h>
#include <kern/video/video.h>
#include <stddef.h>
#include <string.h>

#include <baseline/c_io.h>


int video_init(void) {
	struct VideoInfo_s infoStruct = {
		.status = 0,
		.vbeVersion = 0,
		.softwareRev = 0,
		.videoMemory = 0,
		.capabilities = 0,
		.modeCount = 0,
		.modes = NULL,
		.oem = NULL,
		.vendor = NULL,
		.productName = NULL,
		.productRev = NULL
	};

	
	memcpy(&VIDEO_INFO->info, &infoStruct, sizeof(infoStruct));
	// clear the EDID record
	memset(VIDEO_EDID, 0, sizeof(EDIDRecord));
	// mark the VIDEO_MODE as invalid
	VIDEO_MODE->modeNum = -1;

	return E_VIDEO_SUCCESS;
}


int video_convertVBEInfo(VBEInfo *vbeInfo, VideoInfo *videoInfo) {
	if (vbeInfo == NULL || videoInfo == NULL) {
		return E_VIDEO_ARGNULL;
	}

	videoInfo->info.vbeVersion = vbeInfo->version;
	videoInfo->info.softwareRev = vbeInfo->softwareRev;
	videoInfo->info.videoMemory = vbeInfo->videoMemory;
	videoInfo->info.capabilities = vbeInfo->capabilities;

	videoInfo->info.modes = (uint16_t*)(videoInfo->buf + sizeof(struct VideoInfo_s));

	// emptystr located at the last byte of the VideoInfo block
	char *emptystr = (char*)(videoInfo->buf + VIDEO_INFO_SIZE - 1);
	*emptystr = '\0'; // terminate it

	// default all strings to emptystr in case we run out of space
	videoInfo->info.oem = emptystr;
	videoInfo->info.vendor = emptystr;
	videoInfo->info.productName = emptystr;
	videoInfo->info.productRev = emptystr;

	#define MODES_MAX ((512 - 1 - sizeof(struct VideoInfo_s)) / sizeof(uint16_t))

	// the amount of bytes available for the mode list and strings
	unsigned freebytes = 512 - 1 - sizeof(struct VideoInfo_s);

	uint32_t vbeModePtr = vbeInfo->videoModes;
	uint16_t *modePtr = (uint16_t*)vbe_ptr(vbeModePtr);
	uint32_t modeCount = 0;
	do {
		uint16_t mode = *modePtr;
		if (mode == VBE_MODELIST_TERMINATOR) {
			// End of VBE Mode list
			break;
		}
		videoInfo->info.modes[modeCount] = mode;

		freebytes -= sizeof(uint16_t);
		++modePtr;
		++modeCount;
	} while (modeCount <= MODES_MAX);

	videoInfo->info.modeCount = modeCount;

	#define STRINGCOUNT 4
	char **strings[STRINGCOUNT] = { &videoInfo->info.oem,
	                                &videoInfo->info.vendor,
	                                &videoInfo->info.productName,
	                                &videoInfo->info.productRev };

	char* sources[STRINGCOUNT] = { (char*)vbe_ptr(vbeInfo->oem),
	                               (char*)vbe_ptr(vbeInfo->vendor), 
	                               (char*)vbe_ptr(vbeInfo->productName),
	                               (char*)vbe_ptr(vbeInfo->productRev) }; 

	char *source; // pointer to the source string to copy
	char **sptr; // pointer to string we are copying into
	for (int i = 0; i != STRINGCOUNT; ++i) {
		if (freebytes == 1) {
			// no more room in videoInfo, stop
			return E_VIDEO_INFO_TRUNCATED;
		}
		sptr = strings[i];
		source = sources[i];
		*sptr = (char*)videoInfo->buf + (512 - freebytes);
		size_t slen = strlen(source);
		if (slen > freebytes) {
			slen = freebytes - 1; // always leave 1 byte from empty string
		}
		strcpy(*sptr, source);
		(*sptr)[slen] = '\0'; // terminate the copied string
		freebytes -= slen + 1;
	}


	#undef MODES_MAX
	#undef STRINGCOUNT

	return E_VIDEO_SUCCESS;


}

int video_convertVBEMode(VBEModeInfo *vbe, VideoMode *mode) {
	if (vbe == NULL || mode == NULL) {
		return E_VIDEO_ARGNULL;
	}

	mode->fb.location = vbe->v3.PhysBasePtr;
	mode->fb.width = vbe->v3.XResolution;
	mode->fb.height = vbe->v3.YResolution;
	mode->fb.pitch = vbe->v3.LinBytesPerScanLine;
	mode->fb.bpp = vbe->v3.BitsPerPixel;

	#define MASK(field) ((1 << field) - 1)

	mode->fb.colorspace[VIDEO_COMPONENT_RED] = (struct VideoColorConfig_s){
		.mask = MASK(vbe->v3.LinRedMaskSize),
		.position = vbe->v3.LinRedFieldPosition
	};

	mode->fb.colorspace[VIDEO_COMPONENT_BLUE] = (struct VideoColorConfig_s){
		.mask = MASK(vbe->v3.LinBlueMaskSize),
		.position = vbe->v3.LinBlueFieldPosition
	};

	mode->fb.colorspace[VIDEO_COMPONENT_GREEN] = (struct VideoColorConfig_s){
		.mask = MASK(vbe->v3.LinGreenMaskSize),
		.position = vbe->v3.LinGreenFieldPosition
	};


	#undef MASK

	return E_VIDEO_SUCCESS;
}

int video_dumpInfo(VideoInfo *info) {
	c_puts("Video controller information:\n");
	c_printf(" * VBE Version: %x\n", info->info.vbeVersion);
	c_printf(" * VBE Revision: %d\n", info->info.softwareRev);
	c_printf(" * Video Memory: %d 64KB blocks\n", info->info.videoMemory);
	c_printf(" * Video Modes: %d\n", info->info.modeCount);
	c_printf(" * OEM: %s\n", info->info.oem);
	c_printf(" * Vendor: %s\n", info->info.vendor);
	c_printf(" * Product: %s\n", info->info.productName);
	c_printf(" * Product Revision: %s\n", info->info.productRev);
	return E_VIDEO_SUCCESS;
}

int video_setMode(VideoMode *mode) {

	if (vbe_setMode(mode->modeNum | VBE_MODE_FLAG_LFB, NULL) != E_VESA_SUCCESS) {
		return E_VIDEO_UNSUPPORTED;
	}

	// set the current VIDEO_MODE to this one
	memcpy(VIDEO_MODE, mode, sizeof(VideoMode));

	fb_clear(&VIDEO_MODE->fb, 0);

	return E_VIDEO_SUCCESS;
}