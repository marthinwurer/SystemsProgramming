/**
 * @file: router.c
 * @author: Nathan C. Castle
 * @summary: primary I/O Manager implementation
 **/

#include "router.h"

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

static IO_FILESYSTEM filesystems[IO_OBJ_COUNT];
static IO_DEVICE     devices[IO_OBJ_COUNT];
static IO_MOUNT      mounts[IO_OBJ_COUNT];
static IO_MIDDLEWARE middlewares[IO_OBJ_COUNT];
static IO_MESSAGE    messages[5 * IO_OBJ_COUNT]; //can only have 50 open messages

static short filesystems_next = 0;
static short filesystems_max = IO_OBJ_COUNT -1;
static short devices_next = 0;
static short devices_max = IO_OBJ_COUNT -1;
static short mounts_next = 0;
static short mounts_max = IO_OBJ_COUNT -1;
static short middlewares_next = 0;
static short middlewares_max = IO_OBJ_COUNT -1;
static short messages_next = 0;
static short messages_max = 5 * IO_OBJ_COUNT - 1;

static int32_t IO_HANDLE_NEXT = 0;
static int32_t IO_HANDLE_MAX = 99;
static _io_object_entry HANDLE_TABLE[100];

static short     has_initted = 0; //false
/** Private Functions **/

status_t grab_handle(PIOHANDLE handle){
    int32_t proto_handle = IO_HANDLE_NEXT;
    while( HANDLE_TABLE[proto_handle].handle != NULL){
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
    .owner = NULL
    };
    *handle = proto_handle;
    return E_SUCCESS;
}

status_t grab_table_space(IO_OBJ_TYPE type, int32_t* result){
    return 0; //TODO - don't do this
}

/**
 * @summary: invokes a filter by index; filter is passed the handle to the IO_MESSAGE
 * @pre-condition: handle is valid and refers to an IO_MESSAGE
 * */
status_t call_filter(IOHANDLE handle, int32_t index){
    if (index > middlewares_max){
        return E_OUT_OF_BOUNDS;
    }
    PIO_MIDDLEWARE filter= &middlewares[index];
    if (filter->handle < 0){
        return E_OUT_OF_BOUNDS;
    }
    return filter->execute(handle);
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
    if (path[0] == "\\"){
        ++path; //gets to base path excluding leading slash
    }
    unsigned int index = strpos(path, '\\', 0);
    //iterate through entries in mount table
    for (int entryi = 0; entryi < mounts_max; entryi++){
        if (strlen(mounts[entryi].path) != index){
            continue;
        }
        for (unsigned int i=0; i < index; i++){
            if (mounts[entryi].path[i] != path[i]){
                break;
            }
        }
        *filesystem = mounts[entryi].filesystem;
        *device = mounts[entryi].device;
        return E_SUCCESS;
    }
        //on match, extract handles
        //return
    return E_NO_MATCH;
}
/** Public Functions **/

status_t IO_INIT(){
    for (int i = 0; i < IO_OBJ_COUNT; i++){
        filesystems[i] = _io_fs_init_null();
        devices[i] = _io_dv_init_null();
        mounts[i] = _io_mp_init_null();
        middlewares[i] = _io_md_init_null();
        for (int j =0; j < 5; j++){
            messages[i + j] = _io_msg_init_null();
        }
    }

    for (int i = 0; i < 100; i++){
        HANDLE_TABLE[i] = (struct _ioentry){-1, IO_OBJ_UNKNOWN, 0, NULL, 0};
    }
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
    //set up the table entry
    int32_t table_index;
    stat = grab_table_space(type, &table_index);
    if (stat != E_SUCCESS){
        return stat;
    }
    //set up the handle
    HANDLE_TABLE[handle].type = type; 
    //set up the object
    switch (type) {
        case IO_OBJ_FILESYSTEM:
            //construct object
            filesystems[table_index] = _io_fs_init_handle(handle);
            //update handle table
            HANDLE_TABLE[handle].object = &filesystems[table_index];
            break;
        case IO_OBJ_DEVICE:
            //construct object
            devices[table_index] = _io_dv_init_handle(handle);
            //update handle table
            HANDLE_TABLE[handle].object = &devices[table_index];
            break;
        case IO_OBJ_MIDDLEWARE:
            //construct object
            middlewares[table_index] = _io_md_init_handle(handle);
            //update handle table
            HANDLE_TABLE[handle].object = &middlewares[table_index];
            break;
        case IO_OBJ_MESSAGE:
            //construct object
            messages[table_index] = _io_msg_init_handle(handle);
            //update handle table
            HANDLE_TABLE[handle].object = &messages[table_index];
            break;
        case IO_OBJ_MOUNT:
            //construct object
            mounts[table_index] = _io_mp_init_handle(handle);
            //update handle table
            HANDLE_TABLE[handle].object = &mounts[table_index];
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
    switch (type){
        case IO_OBJ_FILESYSTEM:
            if (index > filesystems_max || index < 0){
                return E_OUT_OF_BOUNDS;
            }
            if (filesystems[index].handle < 0){
                return E_NO_DATA;
            }
            *object = filesystems[index].handle;
            break;
        case IO_OBJ_MIDDLEWARE:
            if (index > middlewares_max || index < 0) {
                return E_OUT_OF_BOUNDS;
            }
            if (middlewares[index].handle < 0) {
                return E_NO_DATA;
            }
            *object = middlewares[index].handle;
            break;
        case IO_OBJ_MOUNT:
            if (index > mounts_max || index < 0) {
                return E_OUT_OF_BOUNDS;
            }
            if (mounts[index].handle < 0) {
                return E_NO_DATA;
            }
            *object = mounts[index].handle;
            break;
        case IO_OBJ_DEVICE:
            if (index > devices_max || index < 0){
                return E_OUT_OF_BOUNDS;
            }
            if (devices[index].handle < 0){
                return E_NO_DATA;
            }
            *object = devices[index].handle;
            break;
        case IO_OBJ_MESSAGE:
            if (index > messages_max || index < 0){
                return E_OUT_OF_BOUNDS;
            }
            if (messages[index].handle < 0){
                return E_NO_DATA;
            }
            *object = messages[index].handle;
            break;
        default:
            return E_NOT_IMPLEMENTED;
            break;
    }
    return E_SUCCESS;
}
status_t IO_LOCK(IOHANDLE handle){
    _io_object_entry* t = &HANDLE_TABLE[handle];
    if ((*t).handle < 0){
        return E_BAD_HANDLE;
    }
    (*t).locked = 1;
    return E_SUCCESS;
}
