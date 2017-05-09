#include <kern/early/realmode.h>
#include <kern/early/memory_map_setup.h>
#include <kern/drivers/ramdisk/ramdisk.h>
#include <kern/drivers/rawfs/raw.h>
#include <kern/io/router.h>


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
    //c_puts("Starting IO Router...");
    IO_INIT();
    //c_puts("Returned IO Router...\n");
    //c_puts("Installing RAMDisk...");
    ramdisk_install();
    //c_puts("Installed\n");
    //c_puts("Installing RAWFS...");
    raw_install();
    //c_puts("Installed!\n");
    while(1) { continue;}	
	return 0;


	return 0;

}
