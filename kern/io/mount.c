#include <kern/io/mount.h>

#define _MC_IO_MP_CNT 5

static int _io_mp_count = _MC_IO_MP_CNT;
static int _io_mp_next = 0;
static IO_MOUNT _IO_MP_TABLE[_MC_IO_MP_CNT];

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
        default:
            return E_BAD_ARG;
    }
    return E_SUCCESS;
}

IO_MOUNT _io_mp_init_null(){
    return (IO_MOUNT) {
        .handle = (IOHANDLE)-1,
        .name = (char*)NULL,
        .path = (char*)NULL,
        .filesystem = (IOHANDLE)-1,
        .device = (IOHANDLE)-1
    };
}

IO_MOUNT _io_mp_init_handle(IOHANDLE handle){
    return (IO_MOUNT) {
        .handle = handle,
        .name = (char*)NULL,
        .path = (char*)NULL,
        .filesystem = (IOHANDLE)-1,
        .device = (IOHANDLE)-1
    };
}

status_t _io_init_mounts() {
    for (int i = 0; i < _io_mp_count; i++){
        _IO_MP_TABLE[i] = _io_mp_init_null();
    }
    return E_SUCCESS;
}

status_t _io_grab_next_mount(PIO_MOUNT* out_p_mount){
    int cursor = _io_mp_next;
    while (_IO_MP_TABLE[cursor].handle != -1){
        cursor = (cursor + 1) % _io_mp_count;
        if (cursor == _io_mp_next){ //we've tried everything; no more space
            return E_OUT_OF_OBJECTS;
        }
    }
    _io_mp_next = (cursor + 1) % _io_mp_count;
    *out_p_mount = &_IO_MP_TABLE[cursor];
    (*out_p_mount)->handle = 0;
    return E_SUCCESS;
}

status_t _io_mp_iterate(PIOHANDLE out, int index) {
    if (index >= _io_mp_count){
        return E_OUT_OF_BOUNDS;
    }
    if (index < 0) {
        return E_OUT_OF_BOUNDS;
    }
    *out = _IO_MP_TABLE[index].handle;
    return E_SUCCESS;
}
status_t _io_mp_getprop(PIO_MOUNT mp, IOPROP prop, void* value, PBSIZE plength){
    //verify length
    if (*plength < 0){
        return E_BAD_ARG;
    }
    //verify value pointer
    if (mp->handle < 0){
        return E_BAD_HANDLE;
    }
    int length = 0;
    //update property
    switch (prop){
        case IOPROP_PATH: ;
            length = strlen(mp->path);
            if (length > *plength) {
                *plength = length;
                return E_MORE_DATA;
            }
            *plength = length;
            strcpy(mp->path, (char*)value);
            break;
        case IOPROP_NAME: ;
            length = strlen(mp->name);
            if (length > *plength) {
                *plength = length;
                return E_MORE_DATA;
            }
            *plength = length;
            strcpy(mp->name, (char*) value);
            break;
        case IOPROP_FILESYSTEM: ;
            *plength = sizeof(IOHANDLE);
            *((IOHANDLE*)value) = mp->filesystem;
            break;
        case IOPROP_DEVICE: ;
            *plength = sizeof(IOHANDLE);
            *((IOHANDLE*)value) = mp->device;
            break;
        default:
            return E_BAD_ARG;
    }
    return E_SUCCESS;
}
