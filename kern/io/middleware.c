#include <kern/io/middleware.h>

#define _MC_IO_MD_CNT 5

static int _io_md_count = _MC_IO_MD_CNT;
static int _io_md_next = 0;
static IO_MIDDLEWARE _IO_MD_TABLE[_MC_IO_MD_CNT];

status_t _io_md_setprop(PIO_MIDDLEWARE md, IOPROP prop, void* value, int32_t length){
    //verify length
    if (length < 0){
        return E_BAD_ARG;
    }
    //verify value pointer
    if (md->handle < 0){
        return E_BAD_HANDLE;
    }
    //update property
    switch (prop){
        case IOPROP_NAME:
            if (md->name == NULL) {
                md->name = malloc(length + 1);
            } else if (strlen(md->name) < length) {
                free(md->name);
                md->name = malloc(length + 1);
            }
            strcpy(md->name, (char*)value);
            break;
        case IOPROP_EXECUTE:
            md->execute = value;
            break;
        default:
            return E_BAD_ARG;
    }
    return E_SUCCESS;

}

IO_MIDDLEWARE _io_md_init_null(){
    return (IO_MIDDLEWARE) {
        .handle = IOHANDLE_NULL,
        .name = (char*)NULL,
        .execute = NULL
    };
}

IO_MIDDLEWARE _io_md_init_handle(IOHANDLE handle){
    return (IO_MIDDLEWARE) {
        .handle = handle,
        .name = (char*)NULL,
        .execute = NULL
    };
}

status_t _io_init_middlewares() {
    for (int i = 0; i < _io_md_count; i++){
        _IO_MD_TABLE[i] = _io_md_init_null();
    }
    return E_SUCCESS;
}

status_t _io_grab_next_middleware(PIO_MIDDLEWARE* out_p_middleware){
    int cursor = _io_md_next;
    while (_IO_MD_TABLE[cursor].handle != IOHANDLE_NULL){
        cursor = (cursor + 1) % _io_md_count;
        if (cursor == _io_md_next){ //we've tried everything; no more space
            return E_OUT_OF_OBJECTS;
        }
    }
    _io_md_next = (cursor + 1) % _io_md_count;
    *out_p_middleware = &_IO_MD_TABLE[cursor];
    (*out_p_middleware)->handle = 0;
    return E_SUCCESS;
}

status_t _io_md_iterate(PIOHANDLE out, int index) {
    if (index >= _io_md_count){
        return E_OUT_OF_BOUNDS;
    }
    if (index < 0) {
        return E_OUT_OF_BOUNDS;
    }
    *out = _IO_MD_TABLE[index].handle;
    return E_SUCCESS;
}
status_t _io_md_getprop(PIO_MIDDLEWARE md, IOPROP prop, void* value, PBSIZE plength){
    //verify length
    if (*plength < 0){
        return E_BAD_ARG;
    }
    //verify value pointer
    if (md->handle < 0){
        return E_BAD_HANDLE;
    }
    //update property
    switch (prop){
        case IOPROP_NAME: ;
            int length = strlen(md->name);
            if (length > *plength) {
                *plength = length;
                return E_MORE_DATA;
            }
            *plength = length;
            strcpy((char*)value, md->name);
            break;
        case IOPROP_EXECUTE:
            *plength = sizeof(void*);
            *((void**)value) = md->execute;
            break;
        default:
            return E_BAD_ARG;
    }
    return E_SUCCESS;

}
