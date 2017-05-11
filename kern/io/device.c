#include <kern/io/device.h>

#define _MC_IO_DV_CNT 5

static int _io_dev_count = _MC_IO_DV_CNT;
static int _io_dev_next = 0;
static IO_DEVICE _IO_DV_TABLE[_MC_IO_DV_CNT];

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
        .read = NULL,
        .write = NULL,
        .finalize = NULL
    };
}

IO_DEVICE _io_dv_init_handle(IOHANDLE handle){
    return (IO_DEVICE) {
        .handle = handle,
        .name = (char*)NULL,
        .read = NULL,
        .write = NULL,
        .finalize = NULL
    };
}

status_t _io_init_devices() {
    for (int i = 0; i < _io_dev_count; i++){
        _IO_DV_TABLE[i] = _io_dv_init_null();
    }
    return E_SUCCESS;
}

status_t _io_grab_next_device(PIO_DEVICE* out_p_device){
    int cursor = _io_dev_next;
    while (_IO_DV_TABLE[cursor].handle != -1){
        cursor = (cursor + 1) % _io_dev_count;
        if (cursor == _io_dev_next){ //we've tried everything; no more space
            return E_OUT_OF_OBJECTS;
        }
    }
    _io_dev_next = (cursor + 1) % _io_dev_count;
    *out_p_device = &_IO_DV_TABLE[cursor];
    (*out_p_device)->handle = 0;
    return E_SUCCESS;
}

status_t _io_dv_iterate(PIOHANDLE out, int index) {
    if (index >= _io_dev_count){
        return E_OUT_OF_BOUNDS;
    }
    if (index < 0) {
        return E_OUT_OF_BOUNDS;
    }
    *out = _IO_DV_TABLE[index].handle;
    return E_SUCCESS;
}
status_t _io_dv_getprop(PIO_DEVICE dv, IOPROP prop, void* value, PBSIZE plength){
    //verify length
    if (*plength < 0){
        return E_BAD_ARG;
    }
    //verify value pointer
    if (dv->handle < 0){
        return E_BAD_HANDLE;
    }
    //update property
    switch (prop){
        case IOPROP_NAME: ;
            int length = strlen(dv->name);
            if (length > *plength) { return E_MORE_DATA; }
            *plength = length;
            strcpy(dv->name, (char*)value);
            return E_SUCCESS;
        case IOPROP_READ:
            *plength = sizeof(void*);
            *((void**)value) = dv->read;
            return E_SUCCESS;
        case IOPROP_WRITE:
            *((void**)value) = dv->write;
            *plength = sizeof(void*);
            return E_SUCCESS;
        case IOPROP_FINALIZE:
            *((void**)value) = dv->finalize;
            *plength = sizeof(void*);
            return E_SUCCESS;
        default:
            return E_BAD_ARG;
    }
    return E_BAD_ARG;
}
