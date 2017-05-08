#include <kern/early/realmode.h>
#include <kern/early/memory_map_setup.h>
#include "../drivers/ramdisk/ramdisk.h"
#include "../drivers/rawfs/raw.h"
#include "../io/router.h"


//
// Main function for the early initialization routine. Any needed BIOS function
// calls should be done here while storing the results somewhere the kernel can
// access it. Note that you cannot use c_io since, like the bootstrap, it is
// not linked with that code. Any errors that occur here are to be handled 
// by the kernel (unless the error is unrecoverable, then just halt).
//
// Nothing has been setup yet, so there should be no problems setting things
// up using the protected-to-real mode int function.
//
int main(void) {


	get_memory_map();
    IO_INIT();
    ramdisk_install();
    raw_install();
	

	return 0;

}
