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

typedef struct _io_message {
    IOHANDLE        handle;
    char*           path;
    void*           filesystem; //PIO_FILESYSTEM
    void*           device; //PIO_DEVICE
    char*           buffer;
    int32_t         length;
    status_t        status;
} IO_MESSAGE, *PIO_MESSAGE;

typedef struct _io_filesystem {
    IOHANDLE        handle;
    char*           name;
    int32_t         created;
    status_t        (*execute)(PIO_MESSAGE msg);
    status_t        (*init)();
    status_t        (*finalize)();
} IO_FILESYSTEM, *PIO_FILESYSTEM;

typedef struct _io_device {
    IOHANDLE        handle;
    char*           name;
    int32_t         created;
    status_t        (*read)(int offset, PBSIZE length, void* buffer);
    status_t        (*write)(int offset, PBSIZE length, void* buffer);
    status_t        (*finalize)();
} IO_DEVICE, *PIO_DEVICE;

typedef struct _io_mount {
    IOHANDLE        handle;
    char*           name;
    char*           path;
    int32_t         created;
    IOHANDLE        filesystem;
    IOHANDLE        device;
} IO_MOUNT, *PIO_MOUNT;

typedef struct _io_middleware {
    IOHANDLE        handle;
    char*           name;
    int32_t         created;
    status_t        (*execute)(PIO_MESSAGE msg);
} IO_MIDDLEWARE, *PIO_MIDDLEWARE;


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
    int32_t index = strpos(path, '\\', 0);
    //iterate through entries in mount table
    for (unsigned int entryi = 0; entryi < mounts_max; entryi++){
        if (strlen(mounts[entryi].path) != index){
            continue;
        }
        for (unsigned int i=0; i < index; i++){
            if (mounts[entryi].path[i] != path[i]){
                break;
            }
        }
        filesystem = &mounts[entryi].filesystem;
        device = &mounts[entryi].device;
        return E_SUCCESS;
    }
        //on match, extract handles
        //return
    return E_NO_MATCH;
}
/** Public Functions **/

status_t IO_INIT(){
    for (int i = 0; i < IO_OBJ_COUNT; i++){
        filesystems[i] = (IO_FILESYSTEM){{-1}};
        devices[i] = (IO_DEVICE){{-1}};
        mounts[i] = (IO_MOUNT){{-1}};
        middlewares[i] = (IO_MIDDLEWARE){{-1}};
        for (int j =0; j < 5; j++){
            messages[i + j] = (IO_MESSAGE){{-1}};
        }
    }

    for (int i = 0; i < 100; i++){
        HANDLE_TABLE[i] = (struct _ioentry){{-1}};
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
            filesystems[table_index] = (IO_FILESYSTEM) {
                .handle = handle,
                .name = NULL,
                .created = NULL, //current time
                .execute = NULL,
                .init = NULL,
                .finalize = NULL
            };
            //update handle table
            HANDLE_TABLE[handle].object = &filesystems[table_index];
            break;
        case IO_OBJ_DEVICE:
            //construct object
            devices[table_index] = (IO_DEVICE) {
                .handle = handle,
                .name = NULL,
                .created = NULL,
                .read = NULL,
                .write = NULL,
                .finalize = NULL
            };
            //update handle table
            HANDLE_TABLE[handle].object = &devices[table_index];
            break;
        case IO_OBJ_MIDDLEWARE:
            //construct object
            middlewares[table_index] = (IO_MIDDLEWARE) {
                .handle = handle,
                .name = NULL,
                .created = NULL,
                .execute = NULL
            };
            //update handle table
            HANDLE_TABLE[handle].object = &middlewares[table_index];
            break;
        case IO_OBJ_MESSAGE:
            //construct object
            messages[table_index] = (IO_MESSAGE) {
                .handle = handle,
                .path = NULL, //char*
                .filesystem = NULL, //PIO_FILESYSTEM
                .device = NULL, //PIO_DEVICE
                .buffer = NULL, //byte*
                .length = NULL, //int32_t
                .status = E_NOT_IMPLEMENTED //status_t
            };
            //update handle table
            HANDLE_TABLE[handle].object = &messages[table_index];
            break;
        case IO_OBJ_MOUNT:
            //construct object
            mounts[table_index] = (IO_MOUNT) {
                .handle = handle,
                .name = NULL, //char*
                .path = NULL, //char*
                .created = NULL, //int32_t
                .filesystem = NULL, //IOHANDLE
                .device = NULL //IOHANDLE
            };
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
    return E_NOT_IMPLEMENTED;
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
            *p_o_m = (IO_MESSAGE) {
                .handle = -1,
                .path = NULL, //char*
                .filesystem = NULL, //PIO_FILESYSTEM
                .device = NULL, //PIO_DEVICE
                .buffer = NULL, //byte*
                .length = NULL, //int32_t
                .status = E_NOT_IMPLEMENTED //status_t
            };
            break;
        case IO_OBJ_FILESYSTEM: ;
            PIO_FILESYSTEM p_o_f = handle_entry->object;
            *p_o_f = (IO_FILESYSTEM) {
                .handle = -1,
                .name = NULL,
                .created = NULL, //current time
                .execute = NULL,
                .init = NULL,
                .finalize = NULL
            };
            break;
        case IO_OBJ_DEVICE: ;
            PIO_DEVICE p_o_d = handle_entry->object;
            *p_o_d = (IO_DEVICE) {
                .handle = -1,
                .name = NULL,
                .created = NULL,
                .read = NULL,
                .write = NULL,
                .finalize = NULL
            };
            break;
        case IO_OBJ_MOUNT: ;
            PIO_MOUNT p_o_o = handle_entry->object;
            *p_o_o = (IO_MOUNT) {
                .handle = -1,
                .name = NULL, //char*
                .path = NULL, //char*
                .created = NULL, //int32_t
                .filesystem = NULL, //IOHANDLE
                .device = NULL //IOHANDLE
            };
            break;
        case IO_OBJ_MIDDLEWARE: ;//standard doesn't like assignments after label...
            PIO_MIDDLEWARE p_o_i = handle_entry->object;
            *p_o_i = (IO_MIDDLEWARE) {
                .handle = -1,
                .name = NULL,
                .created = NULL,
                .execute = NULL
            };

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
    PIOHANDLE fs;
    PIOHANDLE dev;
    status_t stat = match_path(handle, fs, dev);
    ((IO_MESSAGE*)(handle_entry->object))->device = dev;
    ((IO_MESSAGE*)(handle_entry->object))->filesystem = fs;
    //call all filters
    int32_t index = 0;
    //while (call_filter(handle, index) == E_SUCCESS){
    //    index++;
    //    //check status here
    //}
    
    //call filesystem

    //return results
    return E_NOT_IMPLEMENTED;
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
