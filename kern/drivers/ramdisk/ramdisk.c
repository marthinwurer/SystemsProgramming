/**
 * \Author Nathan C. Castle
 * \brief implementation of the ramdisk in-memory IO backing store
 */
#include "ramdisk.h"
#include "../../io/router.h"
#include "../../io/device.h"
#include <kern/memory/memory_map.h>
#include <baseline/common.h>
#define PAGECOUNT 500
static void* pages[PAGECOUNT]; //500 X 4Kb pages
static IOHANDLE handle = -1;

status_t ramdisk_install(){
    //initialize ram space
    for(int i = 0; i < PAGECOUNT; i++){
        pages[i] = get_next_page();
    }
    //init handle
    IO_PROTOTYPE(IO_OBJ_FILESYSTEM, &handle);
    //fill in name
    char* name = "RamFS";
    int32_t namesize = strlen(name);
    IO_UPDATE(handle, IOPROP_NAME, name, &namesize);
    //TODO fill in create date
    //fill in read
    int32_t pointer_size = sizeof(void*);
    IO_UPDATE(handle, IOPROP_READ, &ramdisk_read,&pointer_size); 
    //fill in write
    IO_UPDATE(handle, IOPROP_WRITE, &ramdisk_write, &pointer_size);
    //fill in finalize
    IO_UPDATE(handle, IOPROP_FINALIZE, &ramdisk_finalize, &pointer_size);
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
