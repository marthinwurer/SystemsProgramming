#include <kern/io/message.h>

#define _MC_IO_MS_CNT 50

static int _io_msg_count = _MC_IO_MS_CNT;
static int _io_msg_next = 0;
static IO_MESSAGE _IO_MSG_TABLE[_MC_IO_MS_CNT];

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
        case IOPROP_CURSOR_POSITION:
            msg->offset_prop.offset = (int32_t)value;
            break;
        case IOPROP_PROP:
            msg->offset_prop.property = (IOPROP)value;
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
status_t _io_init_messages() {
    for (int i = 0; i < _io_msg_count; i++){
        _IO_MSG_TABLE[i] = _io_msg_init_null();
    }
    return E_SUCCESS;
}

status_t _io_grab_next_message(PIO_MESSAGE* out_p_msg){
    int cursor = _io_msg_next;
    while (_IO_MSG_TABLE[cursor].handle != -1){
        cursor = (cursor + 1) % _io_msg_count;
        if (cursor == _io_msg_next){ //we've tried everything; no more space
            return E_OUT_OF_OBJECTS;
        }
    }
    _io_msg_next = (cursor + 1) % _io_msg_count;
    *out_p_msg = &_IO_MSG_TABLE[cursor];
    (*out_p_msg)->handle = 0;
    return E_SUCCESS;
}

status_t _io_msg_iterate(PIOHANDLE out, int index) {
    if (index >= _io_msg_count){
        return E_OUT_OF_BOUNDS;
    }
    if (index < 0) {
        return E_OUT_OF_BOUNDS;
    }
    *out = _IO_MSG_TABLE[index].handle;
    return E_SUCCESS;
}
status_t _io_msg_getprop(PIO_MESSAGE msg, IOPROP prop, void* value, PBSIZE plength){
    //verify length
    if (*plength < 0){
        return E_BAD_ARG;
    }
    //verify value pointer
    if (msg->handle < 0){
        return E_BAD_HANDLE;
    }
    //update property
    switch (prop){
        case IOPROP_PATH: ;
            int length = strlen(msg->path);
            if (length > *plength) { 
                *plength = length;
                return E_MORE_DATA; 
            }
            *plength = length;
            strcpy(msg->path, (char*) value);
            return E_SUCCESS;
        case IOPROP_FILESYSTEM:
            return E_BAD_ARG;
        case IOPROP_DEVICE:
            return E_BAD_ARG;
        case IOPROP_BUFFER:
            *plength = sizeof(void*);
            *((void**)value) = msg->buffer;
            return E_SUCCESS;
        case IOPROP_CURSOR_POSITION:
            *plength = sizeof(int32_t);
            *((int32_t*)value) = msg->offset_prop.offset;
            return E_SUCCESS;
        case IOPROP_PROP:
            *plength = sizeof(IOPROP);
            *((IOPROP*)value) = msg->offset_prop.property;
            break;
        default:
            return E_BAD_ARG;
    }
    return E_SUCCESS;
}
