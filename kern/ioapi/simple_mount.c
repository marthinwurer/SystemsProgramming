/**
 * 
 * \brief implementation of a simple API for installing mountpoints
 */

#include <kern/ioapi/simple_mount.h>
#define HANDLED(code) result = code; if (result != E_SUCCESS) { return result;}
status_t install_mount(char* name, char* path, IOHANDLE device, IOHANDLE filesystem){
    status_t result = E_SUCCESS;
    IOHANDLE handle = -1;
    HANDLED(IO_PROTOTYPE(IO_OBJ_MOUNT, &handle));
    int32_t namelen = strlen(name);
    int32_t pathlen = strlen(path);
    HANDLED(IO_UPDATE(handle, IOPROP_NAME, name, &namelen));
    HANDLED(IO_UPDATE(handle, IOPROP_PATH, path, &pathlen));
    int32_t sz = sizeof(void*);
    HANDLED(IO_UPDATE(handle, IOPROP_DEVICE, &device, &sz));
    HANDLED(IO_UPDATE(handle, IOPROP_FILESYSTEM, &filesystem, &sz));
    HANDLED(IO_LOCK(handle));
    return E_SUCCESS;
}
