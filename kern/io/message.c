#include "message.h"

status_t _io_msg_setprop(PIO_MESSAGE msg, IOPROP prop, void* value, int32_t length){
    //verify length
    if (length < 0){
        return E_BAD_ARG;
    }
    //verify value pointer
    if (msg->handle < 0){
        return E_BAD_HANDLE;
    }
    //update property
    switch (prop){
        case IOPROP_PATH:
            strcpy((char*)value, msg->path);
            break;
        case IOPROP_FILESYSTEM:
            msg->filesystem = value;
            break;
        case IOPROP_DEVICE:
            msg->device = value;
            break;
        case IOPROP_BUFFER:
            msg->buffer = value;
            break;
        default:
            return E_BAD_ARG;
    }
    return E_SUCCESS;
}

IO_MESSAGE _io_msg_init_null(){
    return (IO_MESSAGE) {
        .handle = (IOHANDLE)-1,
        .path = (char*)NULL,
        .filesystem = (void*)NULL,
        .device = (void*)NULL,
        .buffer = (void*)NULL,
        .length = (int32_t)NULL,
        .status = E_NO_DATA
    };
}

IO_MESSAGE _io_msg_init_handle(IOHANDLE handle){
    return (IO_MESSAGE) {
        .handle = handle,
        .path = (char*)NULL,
        .filesystem = (void*)NULL,
        .device = (void*)NULL,
        .buffer = (void*)NULL,
        .length = (int32_t)NULL,
        .status = E_NO_DATA
    };
}
