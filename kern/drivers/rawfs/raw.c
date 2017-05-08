/**
 * \Author Nathan C. Castle
 * \brief Implementation of the RAW Filesystem
 */

#include "raw.h"
#include <baseline/common.h>

static IOHANDLE handle = -1;

status_t raw_install(void){
    IO_PROTOTYPE(IO_OBJ_FILESYSTEM, &handle);
    char* name = "RawFS";
    int32_t namesize = strlen(name);
    IO_UPDATE(handle, IOPROP_NAME, name, &namesize);
    //TODO fill in create date
    //fill in execute
    int32_t pointer_size = sizeof(void*);
    IO_UPDATE(handle, IOPROP_INIT, &raw_init, &pointer_size);
    //fill in init
    IO_UPDATE(handle, IOPROP_EXECUTE, &raw_execute, &pointer_size);
    //fill in finalize
    IO_UPDATE(handle, IOPROP_FINALIZE, &raw_finalize, &pointer_size);
    return E_SUCCESS;
}

/**
 * \brief function called by the router upon a new mount point being installed
 */
status_t raw_init(){
    return E_SUCCESS;
}

status_t raw_process_query(PIO_MESSAGE msg) {
    status_t readstatus = E_SUCCESS;
    switch (msg->offset_prop.property) {
        case IOPROP_IS_DIR:
            *(msg->buffer) = 1;
            msg->length = sizeof(int32_t);
            break;
        case IOPROP_COUNT_CHILDREN:;
            int childcount = 0;
            int32_t length = 1;
            while(readstatus == E_SUCCESS){
                readstatus = ((struct _io_device*)(msg->device))->read(childcount, &length, NULL);
                childcount++;
            }
            break;
        default:
            return E_BAD_ARG;
    }
    return E_SUCCESS;
}

/**
 * \brief called by the io router to let the FS handle a message
 */
status_t raw_execute(PIO_MESSAGE msg){
    /*
     * Path Format
     * \root\<#block offset>
     */
    int32_t count_nodes = io_path_count_nodes(msg->path);
    if (count_nodes != 2 && count_nodes != 1){
        return E_BAD_ARG;
    }
    //discard first node (the mountpoint):
    char newpath[strlen(msg->path)];
    io_path_disc_n_nodes(msg->path, 1, newpath);
    int32_t sector_id = -1;
    int32_t block_count = 1;
    status_t result = atoi(newpath, &sector_id);
    //verify path
    if (result != E_SUCCESS) { return result; }
    //execute
    switch (msg->ioctl) {
        case IOCTL_READ: //writes raw data to block
            return ((struct _io_device*)(msg->device))->read(sector_id, &block_count, msg->buffer);
        case IOCTL_APPEND:
            return E_BAD_ARG; //undefined behavior
        case IOCTL_WRITE: //writes raw data to block
            return ((struct _io_device*)(msg->device))->write(sector_id, &block_count, msg->buffer);
        case IOCTL_EXPAND:
            return E_BAD_ARG; //undefined behavior
        case IOCTL_DELETE: //zeroes out sector
            break;
        case IOCTL_QUERY: //queries metadata; chiefly # of sectors
            if (count_nodes != 1) { //can only operate on root
                return E_BAD_ARG;
            }
            return raw_process_query(msg);
        case IOCTL_CREATE:
            return E_BAD_ARG; //undefined behavior
        case IOCTL_ENUMERATE:
            return E_BAD_ARG; //undefined behavior
        case IOCTL_IDENTIFY:
            return E_BAD_ARG; //undefined behavior
        default:
            return E_BAD_ARG;
    }
    return E_SUCCESS;
}

/**
 * \brief function called by the IO router to destroy the FS and free its resources
 */
status_t raw_finalize(){
    return E_SUCCESS;
}
