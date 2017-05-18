/*
** SCCS ID:	@(#)main.c	1.3	03/15/05
**
** File:	main.c
**
** Author:	K. Reek
**
** Contributor:	Warren R. Carithers
**
** Description:	Dummy main program
** DEPRECATED
*/

#include <baseline/support.h>
#include <kern/vga13/vga13.h>
#include <kern/vgacolor.h>
#include <kern/realmode.h>
#include <baseline/c_io.h>

#include <kern/vesa/VBEInfo.h>
#include <kern/vesa/vbe.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <kern/memory/memory_map.h>
#include <baseline/support.h>

static VBEModeInfo bestRes;
static VBEModeInfo temp;


static uint16_t MODELIST[100];

#define physAddr(segment, offset) ((segment * 0x10) + offset)

//#define vbePtr(ptr) (((ptr >> 16) * 0x10) + (ptr & 0xFFFF))



int main(void) {

	c_clearscreen();




	__init_int32();

	VBEInfo info;
	uint16_t vbeResult;
	int result = vbe_getInfo(&info, &vbeResult);


	if (result) {
		c_printf("vbe_getInfo failed (%x)\n", vbeResult);
	} else {
		c_printf("vbe_getInfo success (%x)\n", vbeResult);
		c_printf("Version: %x\n", info.version);
		c_printf("Video Memory: %d 64KB blocks\n", info.videoMemory);
		c_printf("OEM: %s (0x%x)\n", (const char *)vbe_ptr(info.oem), info.oem);
		c_printf("Vendor: %s (0x%x)\n", (const char *)vbe_ptr(info.vendor), info.vendor);
		
		//c_printf("Supported Modes:\n");
		uint16_t segment = info.videoModes >> 16;
		uint16_t offset = info.videoModes & 0xFFFF;
		uint16_t *modes = (uint16_t*)physAddr(segment, offset);
		c_printf("%x = %x:%x = %x\n", info.videoModes, segment, offset, modes);

		int modeCount = 0;
		for (int i = 0; i != 100; ++i) {
			uint16_t mode = modes[i];
			if (mode == 0xFFFF) {
				modeCount = i;
				break;
			}

			MODELIST[i] = mode;
		}

		c_puts("Searching for highest resolution...");
		memset(&bestRes, 0, sizeof(VBEModeInfo));

		for (int i = 0; i != modeCount; ++i) {
			uint16_t mode = MODELIST[i];
			
			if (vbe_getModeInfo(mode, &temp, NULL) == VBE_SUCCESS) {
				//c_printf("%dx%d ", temp.width, temp.height);
				if (temp.width >= bestRes.width && temp.height >= bestRes.height && temp.bpp >= bestRes.bpp) {
					memcpy(&bestRes, &temp, sizeof(VBEModeInfo));
				}
			}

		}

		c_puts("Done\n");

		c_printf("Available Modes: %d\n", modeCount);

		c_puts("Best Mode:\n");
		c_printf("    Width: %d\n", bestRes.width);
		c_printf("    Height: %d\n", bestRes.height);
		c_printf("    bpp: %d\n", bestRes.bpp);
		c_printf("    Framebuffer: 0x%x\n", bestRes.framebuffer);

		

		uint16_t currentMode;
		if (vbe_currentMode(&currentMode, NULL) == VBE_SUCCESS) {
			c_printf("Current Mode: %x\n", currentMode);

			vbe_getModeInfo(currentMode, &temp, NULL);
			c_printf("    Width: %d\n", temp.width);
			c_printf("    Height: %d\n", temp.height);
			c_printf("    bpp: %d\n", temp.bpp);
			c_printf("    Framebuffer: 0x%x\n", temp.framebuffer);
		}

	}
	return 0;
}
