#include "middleware.h"

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
            strcpy((char*)value, md->name);
            break;
        case IOPROP_CREATED:
            return E_BAD_ARG;
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
        .handle = (IOHANDLE)-1,
        .name = (char*)NULL,
        .created = (int32_t)NULL,
        .execute = NULL
    };
}

IO_MIDDLEWARE _io_md_init_handle(IOHANDLE handle){
    return (IO_MIDDLEWARE) {
        .handle = handle,
        .name = (char*)NULL,
        .created = (int32_t)NULL,
        .execute = NULL
    };
}
