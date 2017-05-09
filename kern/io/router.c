/**
 * @file: router.c
 * @author: Nathan C. Castle
 * @summary: primary I/O Manager implementation
 **/

#include <kern/io/router.h>

#define IO_OBJ_COUNT 10

/** Private Data Structures **/

/** 
 * @summary: entry in the internal handle table
 * */
typedef struct _ioentry {
    IOHANDLE        handle; //ID of the handle
    IO_OBJ_TYPE     type;
    short           locked;
    void*           object; //pointer to object
    pid_t           owner;
} _io_object_entry;

/** Private State **/

static int32_t IO_HANDLE_NEXT = 0;
static int32_t IO_HANDLE_MAX = 99;
static _io_object_entry HANDLE_TABLE[100];

static short     has_initted = 0; //false
/** Private Functions **/

status_t grab_handle(PIOHANDLE handle){
    int32_t proto_handle = IO_HANDLE_NEXT;
    while(HANDLE_TABLE[proto_handle].handle != -1){
        proto_handle++;
        if (proto_handle > IO_HANDLE_MAX){
            proto_handle = 0;
        }
        if (proto_handle == IO_HANDLE_NEXT){
            return E_NO_MORE_HANDLES;
        }
    }
    IO_HANDLE_NEXT = proto_handle++;
    if (IO_HANDLE_NEXT > IO_HANDLE_MAX){
        IO_HANDLE_NEXT = 0;
    }

    HANDLE_TABLE[proto_handle] = (struct _ioentry) {
    .handle = proto_handle,
    .type = IO_OBJ_UNKNOWN,
    .locked = 0, //false
    .object = NULL,
    .owner = 0
    };
    *handle = proto_handle;
    return E_SUCCESS;
}

/**
 * @summary: invokes a filter by index; filter is passed the handle to the IO_MESSAGE
 * @pre-condition: handle is valid and refers to an IO_MESSAGE
 * */
status_t call_filter(IOHANDLE handle, int32_t index){
    IOHANDLE middleware_handle = -1;
    status_t stat = _io_md_iterate(&middleware_handle, index);
    if (stat != E_SUCCESS) { return stat; }
    PIO_MIDDLEWARE filter= HANDLE_TABLE[middleware_handle].object;
    PIO_MESSAGE pm = HANDLE_TABLE[handle].object;
    return filter->execute(pm);
}
/**
 *
 * @summary: matches an IO Message's path with the filesystem or device matching the mount
 * @pre-condition: handle is already verified and refers to a valid IO Message
 * */
status_t match_path(IOHANDLE handle, PIOHANDLE filesystem, PIOHANDLE device){
    //skip handle verification;

    //extract base path
    struct _ioentry* obj = &HANDLE_TABLE[handle];
    PIO_MESSAGE msgpath = obj->object;
    char* path = msgpath->path;
    if (path[0] == '\\'){
        ++path; //gets to base path excluding leading slash
    }
    unsigned int index = strpos(path, '\\', 0);
    //iterate through entries in mount table
    int iter_val = 0;
    IOHANDLE mount_handle = -1;
    int mismatch = 0;
    while (_io_mp_iterate(&mount_handle, iter_val) == E_SUCCESS) {
        char* mountpath = ((PIO_MOUNT)HANDLE_TABLE[mount_handle].object)->path;
        if (strlen(mountpath) == index) {
            for (unsigned int i=0; i < index; i++){
                if (mountpath[i] != path[i]){
                    mismatch++;
                }
            }
            if (mismatch == 0){
                *filesystem = ((PIO_MOUNT)HANDLE_TABLE[mount_handle].object)->filesystem;
                *device = ((PIO_MOUNT)HANDLE_TABLE[mount_handle].object)->device;
                return E_SUCCESS;
            }
        }
        iter_val++;
        mismatch = 0;
    }
    return E_NO_MATCH;
}
/** Public Functions **/

status_t IO_INIT(){
    status_t stat = E_SUCCESS;
    stat = _io_init_filesystems();
    if (stat != E_SUCCESS) { return stat; }
    stat = _io_init_devices();
    if (stat != E_SUCCESS) { return stat; }
    stat = _io_init_messages();
    if (stat != E_SUCCESS) { return stat; }
    stat = _io_init_middlewares();
    if (stat != E_SUCCESS) { return stat; }
    stat = _io_init_mounts();
    if (stat != E_SUCCESS) { return stat; }
    has_initted = 1; //true
    return E_SUCCESS;
}

status_t IO_PROTOTYPE(IO_OBJ_TYPE type, PIOHANDLE out_handle){
    // Make sure we've set up
    if (!has_initted){
        IO_INIT();
    }
    //grab a handle
    IOHANDLE handle;
    status_t stat = grab_handle(&handle);
    if (stat != E_SUCCESS) {
        return stat;
    }
    //set up the handle
    HANDLE_TABLE[handle].type = type; 
    //set up the object
    switch (type) {
        case IO_OBJ_FILESYSTEM: ;
            //construct object
            PIO_FILESYSTEM pfs = NULL;
            stat = _io_grab_next_filesystem(&pfs);
            if (stat != E_SUCCESS) { return stat; }
            *pfs = _io_fs_init_handle(handle);
            //update handle table
            HANDLE_TABLE[handle].object = pfs;
            break;
        case IO_OBJ_DEVICE: ;
            //construct object
            PIO_DEVICE pdv = NULL;
            stat = _io_grab_next_device(&pdv);
            if (stat != E_SUCCESS) { return stat; }
            *pdv = _io_dv_init_handle(handle);
            //update handle table
            HANDLE_TABLE[handle].object = pdv;
            break;
        case IO_OBJ_MIDDLEWARE: ;
            //construct object
            PIO_MIDDLEWARE pmd = NULL;
            stat = _io_grab_next_middleware(&pmd);
            if (stat != E_SUCCESS) { return stat; }
            *pmd = _io_md_init_handle(handle);
            //update handle table
            HANDLE_TABLE[handle].object = pmd;
            break;
        case IO_OBJ_MESSAGE: ;
            //construct object
            PIO_MESSAGE pms = NULL;
            stat = _io_grab_next_message(&pms);
            if (stat != E_SUCCESS) { return stat; }
            *pms = _io_msg_init_handle(handle);
            //update handle table
            HANDLE_TABLE[handle].object = pms;
            break;
        case IO_OBJ_MOUNT: ;
            //construct object
            PIO_MOUNT pmp = NULL;
            stat = _io_grab_next_mount(&pmp);
            if (stat != E_SUCCESS) { return stat; }
            *pmp = _io_mp_init_handle(handle);
            //update handle table
            HANDLE_TABLE[handle].object = pmp;
            break;
        default:
            return E_BAD_TYPE;
    }
    *out_handle = handle;
    return E_SUCCESS;
}

status_t IO_UPDATE(IOHANDLE handle, IOPROP property, void* value, PBSIZE length){
    //verify handle
    if (handle < 0 || handle > IO_HANDLE_MAX) {
        return E_BAD_HANDLE;
    }
    struct _ioentry* handle_entry = &HANDLE_TABLE[handle];
    if (handle_entry->handle != handle){
        return E_BAD_HANDLE;
    }
    if (handle_entry->locked != 0){
        return E_LOCKED;
    }
    status_t result;
    switch (handle_entry->type){
        case IO_OBJ_FILESYSTEM:
            result = _io_fs_setprop(handle_entry->object, property, value, *length);
            break;
        case IO_OBJ_MOUNT:
            result = _io_mp_setprop(handle_entry->object, property, value, *length);
            break;
        case IO_OBJ_DEVICE:
            result = _io_dv_setprop(handle_entry->object, property, value, *length);
            break;
        case IO_OBJ_MESSAGE:
            result = _io_msg_setprop(handle_entry->object, property, value, *length);
            break;
        case IO_OBJ_MIDDLEWARE:
            result =_io_md_setprop(handle_entry->object, property, value, *length);
            break;
        default:
            result = E_NOT_IMPLEMENTED;
            break;
    }
    return result;
}

status_t IO_DELETE(IOHANDLE handle){
    //verify handle
    if (handle < 0 || handle > IO_HANDLE_MAX) {
        return E_BAD_HANDLE;
    }
    struct _ioentry* handle_entry = &HANDLE_TABLE[handle];
    if ((*handle_entry).handle != handle){
        return E_BAD_HANDLE;
    }
    //identify object type
    IO_OBJ_TYPE object_type = (*handle_entry).type;
    //zero out entry
    switch (object_type) {
        case IO_OBJ_MESSAGE: ;
            PIO_MESSAGE p_o_m = handle_entry->object;
            *p_o_m = _io_msg_init_null();
        case IO_OBJ_FILESYSTEM: ;
            PIO_FILESYSTEM p_o_f = handle_entry->object;
            *p_o_f = _io_fs_init_null();
            break;
        case IO_OBJ_DEVICE: ;
            PIO_DEVICE p_o_d = handle_entry->object;
            *p_o_d = _io_dv_init_null();
            break;
        case IO_OBJ_MOUNT: ;
            PIO_MOUNT p_o_o = handle_entry->object;
            *p_o_o = _io_mp_init_null();
            break;
        case IO_OBJ_MIDDLEWARE: ;//standard doesn't like assignments after label...
            PIO_MIDDLEWARE p_o_i = handle_entry->object;
            *p_o_i = _io_md_init_null();
            break;
        default:
            return E_BAD_ARG;
            break;
    }
    //reclaim handle & object table entries
    (*handle_entry).handle = -1;
    (*handle_entry).locked = 0;
    return E_NOT_IMPLEMENTED;
}

status_t IO_EXECUTE(IOHANDLE handle){
    //verify handle
    if (handle < 0 || handle > IO_HANDLE_MAX) {
        return E_BAD_HANDLE;
    }
    struct _ioentry* handle_entry = &HANDLE_TABLE[handle];
    if (handle_entry->handle != handle){
        return E_BAD_HANDLE;
    }

    //ensure handle is IO Message
    if (handle_entry->type != IO_OBJ_MESSAGE){
        return E_BAD_HANDLE;
    }
    //associate file system & device
    PIOHANDLE fs = NULL;
    PIOHANDLE dev = NULL;
    status_t stat = match_path(handle, fs, dev);
    if (stat != E_SUCCESS){
        return stat;
    }
    PIO_MESSAGE pmessage = handle_entry->object;
    pmessage->device = dev;
    pmessage->filesystem = fs;
    //call all filters
    int32_t index = 0;
    while (call_filter(handle, index) == E_SUCCESS){
        index++;
        //check status here
        if (pmessage->status == E_CANCELED){
            return E_CANCELED;
        }
    }
    
    //call filesystem & return result
    PIO_FILESYSTEM pfs = HANDLE_TABLE[*fs].object;
    return pfs->execute(pmessage);
}

status_t IO_ENUMERATE(IO_OBJ_TYPE type, int index, PIOHANDLE object){
    IOHANDLE result = -1;
    status_t stat = E_SUCCESS;
    switch (type){
        case IO_OBJ_FILESYSTEM:
            stat = _io_fs_iterate(&result, index);
            break;
        case IO_OBJ_MIDDLEWARE:
            stat = _io_md_iterate(&result, index);
            break;
        case IO_OBJ_MOUNT:
            stat = _io_mp_iterate(&result, index);
            break;
        case IO_OBJ_DEVICE:
            stat = _io_dv_iterate(&result, index);
            break;
        case IO_OBJ_MESSAGE:
            stat = _io_msg_iterate(&result, index);
            break;
        default:
            return E_NOT_IMPLEMENTED;
            break;
    }
    *object = result;
    return stat;
}
status_t IO_LOCK(IOHANDLE handle){
    if (handle < 0) { return E_BAD_HANDLE; }
    _io_object_entry* t = &HANDLE_TABLE[handle];
    if ((*t).handle < 0) { return E_BAD_HANDLE; }
    (*t).locked = 1;
    return E_SUCCESS;
}
status_t IO_INTERROGATE(IOHANDLE handle, IOPROP prop, void* buffer, PBSIZE plength){
    if (handle < 0) { return E_BAD_HANDLE; }
    _io_object_entry* t = &HANDLE_TABLE[handle];
    if (t->handle < 0) { return E_BAD_HANDLE; }
    switch (t->type) {
        case IO_OBJ_DEVICE:
            return _io_dv_getprop(t->object, prop, buffer, plength);
        case IO_OBJ_MESSAGE:
            return _io_msg_getprop(t->object, prop, buffer, plength);
        case IO_OBJ_FILESYSTEM:
            return _io_fs_getprop(t->object, prop, buffer, plength);
        case IO_OBJ_MIDDLEWARE:
            return _io_md_getprop(t->object, prop, buffer, plength);
        case IO_OBJ_MOUNT:
            return _io_mp_getprop(t->object, prop, buffer, plength);
        default:
            return E_BAD_ARG;
    }
    return E_BAD_ARG;
}
