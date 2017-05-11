/**
 * \Author Nathan C. Castle
 * \brief implementation of the ramdisk in-memory IO backing store
 */
#include <kern/drivers/ramdisk/ramdisk.h>
#include <kern/io/router.h>
#include <kern/io/device.h>
#include <kern/memory/memory_map.h>
#include <baseline/common.h>
#define PAGECOUNT 500
static void* pages[PAGECOUNT]; //500 X 4Kb pages
static IOHANDLE handle = IOHANDLE_NULL;

status_t ramdisk_install(){
    ENABLEHANDLERS
    //initialize ram space
    for(int i = 0; i < PAGECOUNT; i++){
        pages[i] = get_next_page();
    }
    //init handle
    IO_PROTOTYPE(IO_OBJ_DEVICE, &handle);
    //fill in name
    char* name = "RamDisk";
    HANDLED(IO_UPDATE_STR(handle, IOPROP_NAME, name));
    //TODO fill in create date
    //fill in read
    HANDLED(IO_UPDATE_VOID(handle, IOPROP_READ, &ramdisk_read)); 
    //fill in write
    HANDLED(IO_UPDATE_VOID(handle, IOPROP_WRITE, &ramdisk_write));
    //fill in finalize
    HANDLED(IO_UPDATE_VOID(handle, IOPROP_FINALIZE, &ramdisk_finalize));
    IO_LOCK(handle); //we're done here
    return E_SUCCESS;
}

//offset: block offset into data
//length: number of blocks to read
//buffer: storage
status_t ramdisk_read(int offset, PBSIZE length, void* buffer){
    int32_t starting_length = *length;
    int32_t read_length = 0;
    if (starting_length < 0 || offset < 0){
        return E_BAD_ARG; //disallow backward reads
    }
    if (offset + starting_length >= PAGECOUNT) {
        return E_OUT_OF_BOUNDS; //disallow reading out of bounds
    }
    char* pointer_source = pages[offset];
    char* pointer_dest = buffer;
    while (read_length < starting_length){
        memcpy(pointer_dest, pointer_source, 4096);
        *length = ++read_length; //keep up to date in case something goes wrong
        pointer_dest += 4096;
        pointer_source = pages[offset + read_length];
    }
    return E_SUCCESS;
}

status_t ramdisk_write(int offset, PBSIZE length, void* buffer){
    int32_t starting_length = *length;
    int32_t write_length = 0;
    if (starting_length < 0 || offset < 0) {
        return E_BAD_ARG;
    }
    if (offset + starting_length >= PAGECOUNT) {
        return E_OUT_OF_BOUNDS; //disallow
    }
    char* pointer_source = buffer;
    char* pointer_dest = pages[offset];
    while (write_length < starting_length) {
        memcpy(pointer_dest, pointer_source, 4096);
        *length = ++write_length;
        pointer_dest = pages[offset + write_length];
        pointer_source += 4096;
    }
    return E_SUCCESS;
}

status_t ramdisk_finalize(void){
    for (int i = 0; i < PAGECOUNT; i++){
        free_page(pages[i]);
    }
    return IO_DELETE(handle);
}
