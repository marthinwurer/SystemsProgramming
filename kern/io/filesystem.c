#include <kern/io/filesystem.h>

#define _MC_IO_FS_CNT 5
static int _io_fs_count = _MC_IO_FS_CNT;
static int _io_fs_next = 0;
static IO_FILESYSTEM _IO_FS_TABLE[_MC_IO_FS_CNT];

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
            length = strlen((char*)value);
            if (fs->name == NULL) {
                fs->name = malloc(length + 1);
            } else if (strlen(fs->name) < length) {
                free(fs->name);
                fs->name = malloc(length + 1);
            }
            strcpy(fs->name, (char*)value);
            break;
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
        .handle = IOHANDLE_NULL,
        .name = (char*)NULL,
        .execute = NULL,
        .init = NULL,
        .finalize = NULL
    };
}

IO_FILESYSTEM _io_fs_init_handle(IOHANDLE handle) {
    return (IO_FILESYSTEM) {
        .handle = handle,
        .name = (char*)NULL,
        .execute = NULL,
        .init = NULL,
        .finalize = NULL
    };
}

status_t _io_init_filesystems() {
    for (int i = 0; i < _io_fs_count; i++){
        _IO_FS_TABLE[i] = _io_fs_init_null();
    }
    return E_SUCCESS;
}

status_t _io_grab_next_filesystem(PIO_FILESYSTEM* out_p_filesystem){
    int cursor = _io_fs_next;
    while (_IO_FS_TABLE[cursor].handle != IOHANDLE_NULL){
        cursor = (cursor + 1) % _io_fs_count;
        if (cursor == _io_fs_next){ //we've tried everything; no more space
            return E_OUT_OF_OBJECTS;
        }
    }
    _io_fs_next = (cursor + 1) % _io_fs_count;
    *out_p_filesystem = &_IO_FS_TABLE[cursor];
    (*out_p_filesystem)->handle = 0;
    return E_SUCCESS;
}

status_t _io_fs_iterate(PIOHANDLE out, int index) {
    if (index >= _io_fs_count){
        return E_OUT_OF_BOUNDS;
    }
    if (index < 0) {
        return E_OUT_OF_BOUNDS;
    }
    *out = _IO_FS_TABLE[index].handle;
    return E_SUCCESS;
}
status_t _io_fs_getprop(PIO_FILESYSTEM fs, IOPROP prop, void* value, PBSIZE plength){
    //verify length
    if (*plength < 0){
        return E_BAD_ARG;
    }
    //verify value pointer
    if (fs->handle < 0){
        return E_BAD_HANDLE;
    }
    //update property
    switch (prop){
        case IOPROP_NAME: ;
            int length = strlen(fs->name);
            if (length > *plength) {
                *plength = length;
                return E_MORE_DATA;
            }
            *plength = length;
            strcpy((char*)value, fs->name);
            return E_SUCCESS;
        case IOPROP_EXECUTE:
            *((void**)value) = fs->execute;
            *plength = sizeof(void*);
            return E_SUCCESS;
        case IOPROP_INIT:
            *((void**)value) = fs->init;
            *plength = sizeof(void*);
            return E_SUCCESS;
        case IOPROP_FINALIZE:
            *((void**)value) = fs->finalize;
            *plength = sizeof(void*);
            return E_SUCCESS;
        default:
            return E_BAD_ARG;
    }
    return E_SUCCESS;
}
