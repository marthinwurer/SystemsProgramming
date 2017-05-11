/**
 * 
 * \brief implementation of a simple API for installing mountpoints
 */

#include <kern/ioapi/simple_mount.h>

status_t install_mount(char* name, char* path, IOHANDLE device, IOHANDLE filesystem){
    ENABLEHANDLERS
    IOHANDLE handle = IOHANDLE_NULL;
    HANDLED(IO_PROTOTYPE(IO_OBJ_MOUNT, &handle));
    HANDLED(IO_UPDATE_STR(handle, IOPROP_NAME, name));
    HANDLED(IO_UPDATE_STR(handle, IOPROP_PATH, path));
    HANDLED(IO_UPDATE_VALINT(handle, IOPROP_DEVICE, (int32_t)device));
    HANDLED(IO_UPDATE_VALINT(handle, IOPROP_FILESYSTEM, filesystem));
    //HANDLED(IO_LOCK(handle));
    return E_SUCCESS;
}

status_t uninstall_mount(IOHANDLE handle) {
    return IO_DELETE(handle);
}
