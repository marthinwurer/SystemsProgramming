#include "filesystem.h"

status_t _io_fs_setprop(PIO_FILESYSTEM fs, IOPROP prop, void* value, int32_t length){
    //verify length
    if (length < 0){
        return E_BAD_ARG;
    }
    //verify value pointer
    if (fs->handle < 0){
        return E_BAD_HANDLE;
    }
    //update property
    switch (prop){
        case IOPROP_NAME:
            strcpy((char*)value, fs->name);
            break;
        case IOPROP_CREATED:
            return E_BAD_ARG;
        case IOPROP_EXECUTE:
            fs->execute = value;
            break;
        case IOPROP_INIT:
            fs->init = value;
            break;
        case IOPROP_FINALIZE:
            fs->finalize = value;
            break;
        default:
            return E_BAD_ARG;
    }
    return E_SUCCESS;
}

IO_FILESYSTEM _io_fs_init_null(){
    return (IO_FILESYSTEM) {
        .handle = (IOHANDLE)-1,
        .name = (char*)NULL,
        .created = (int32_t)NULL,
        .execute = NULL,
        .init = NULL,
        .finalize = NULL
    };
}

IO_FILESYSTEM _io_fs_init_handle(IOHANDLE handle) {
    return (IO_FILESYSTEM) {
        .handle = handle,
        .name = (char*)NULL,
        .created = (int32_t)NULL,
        .execute = NULL,
        .init = NULL,
        .finalize = NULL
    };
}
