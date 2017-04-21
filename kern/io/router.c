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
/** Public Functions **/

status_t IO_INIT(){
    for (int i = 0; i < IO_OBJ_COUNT; i++){
        filesystems[i] = (IO_FILESYSTEM){{ 0}};
        devices[i] = (IO_DEVICE){{0}};
        mounts[i] = (IO_MOUNT){{0}};
        middlewares[i] = (IO_MIDDLEWARE){{0}};
        for (int j =0; j < 5; j++){
            messages[i + j] = (IO_MESSAGE){{0}};
        }
    }

    for (int i = 0; i < 100; i++){
        HANDLE_TABLE[i] = (struct _ioentry){{0}};
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
    return E_NOT_IMPLEMENTED;
}

status_t IO_EXECUTE(IOHANDLE handle){
    return E_NOT_IMPLEMENTED;
}

status_t IO_ENUMERATE(IO_OBJ_TYPE type, int index, PIOHANDLE object){
    return E_NOT_IMPLEMENTED;
}
status_t IO_LOCK(IOHANDLE handle){
    return E_NOT_IMPLEMENTED;
}
