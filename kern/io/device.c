#include "device.h"

status_t _io_dv_setprop(PIO_DEVICE dv, IOPROP prop, void* value, int32_t length){
    //verify length
    if (length < 0){
        return E_BAD_ARG;
    }
    //verify value pointer
    if (dv->handle < 0){
        return E_BAD_HANDLE;
    }
    //update property
    switch (prop){
        case IOPROP_NAME:
            strcpy((char*)value, dv->name);
            break;
        case IOPROP_CREATED:
            return E_BAD_ARG;
        case IOPROP_READ:
            dv->read = value;
            break;
        case IOPROP_WRITE:
            dv->write = value;
            break;
        case IOPROP_FINALIZE:
            dv->finalize = value;
            break;
        default:
            return E_BAD_ARG;
    }
    return E_SUCCESS;
}

IO_DEVICE _io_dv_init_null(){
    return (IO_DEVICE) {
        .handle = (IOHANDLE)-1,
        .name = (char*)NULL,
        .created = (int32_t)NULL,
        .read = NULL,
        .write = NULL,
        .finalize = NULL
    };
}

IO_DEVICE _io_dv_init_handle(IOHANDLE handle){
    return (IO_DEVICE) {
        .handle = handle,
        .name = (char*)NULL,
        .created = (int32_t)NULL,
        .read = NULL,
        .write = NULL,
        .finalize = NULL
    };
}
