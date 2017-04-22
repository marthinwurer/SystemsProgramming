#include "mount.h"

status_t _io_mp_setprop(PIO_MOUNT mp, IOPROP prop, void* value, int32_t length){
    //verify length
    if (length < 0){
        return E_BAD_ARG;
    }
    //verify value pointer
    if (mp->handle < 0){
        return E_BAD_HANDLE;
    }
    //update property
    switch (prop){
        case IOPROP_PATH:
            strcpy((char*)value, mp->path);
            break;
        case IOPROP_NAME:
            strcpy((char*)value, mp->name);
            break;
        case IOPROP_FILESYSTEM: ;
            PIOHANDLE pvf = value;
            mp->filesystem = *pvf;
            break;
        case IOPROP_DEVICE: ;
            PIOHANDLE pvd = value;
            mp->device = *pvd;
            break;
        case IOPROP_CREATED: ;
            int32_t* pvc = value;
            mp->created = *pvc;
            break;
        default:
            return E_BAD_ARG;
    }
    return E_SUCCESS;
}

IO_MOUNT _io_mp_init_null(){
    return (IO_MOUNT) {
        .handle = (IOHANDLE)-1,
        .name = (char*)NULL,
        .created = (int32_t)NULL,
        .path = (char*)NULL,
        .filesystem = (IOHANDLE)-1,
        .device = (IOHANDLE)-1
    };
}

IO_MOUNT _io_mp_init_handle(IOHANDLE handle){
    return (IO_MOUNT) {
        .handle = handle,
        .name = (char*)NULL,
        .created = (int32_t)NULL,
        .path = (char*)NULL,
        .filesystem = (IOHANDLE)-1,
        .device = (IOHANDLE)-1
    };
}
