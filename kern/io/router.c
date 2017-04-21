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
    IOHANDLE        handle_id; //ID of the handle
    IO_OBJ_TYPE     type;
    bool            locked;
    void*           object; //pointer to object
    pid_t           owner;
} _io_object_entry;

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

typedef struct _io_message {
    IOHANDLE        handle;
    char*           path;
    PIO_FILESYSTEM  filesystem;
    PIO_DEVICE      device;
    byte*           buffer;
    int32_t         length;
    status_t        status;
} IO_MESSAGE, *PIO_MESSAGE;

/** Private State **/

static IO_FILESYSTEM filesystems[IO_OBJ_COUNT];
static IO_DEVICE     devices[IO_OBJ_COUNT];
static IO_MOUNT      mounts[IO_OBJ_COUNT];
static IO_MIDDLEWARE middlewares[IO_OBJ_COUNT];
static IO_MESSAGE    messages[5 * IO_OBJ_COUNT]; //can only have 50 open messages

static short filesystems_next = 0;
static short filesystems_max = OBJ_COUNT -1;
static short devices_next = 0;
static short devices_max = OBJ_COUNT -1;
static short mounts_next = 0;
static short mounts_max = OBJ_COUNT -1;
static short middlewares_next = 0;
static short middlewares_max = OBJ_COUNT -1;
static short messages_next = 0;
static short messages_max = 5 * OBJ_COUNT - 1;

static int32_t IO_HANDLE_NEXT = 0;
static int32_t IO_HANDLE_MAX = 99;
static _io_object_entry HANDLE_TABLE[100];

static bool     has_initted = false;
/** Private Functions **/

status_t grab_handle(PIO_HANDLE handle){
    int32_t proto_handle = IO_HANDLE_NEXT;
    while( HANDLE_TABLE[proto_handle] != NULL){
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

    HANDLE_TABLE[proto_handle] = _io_object_entry{
    .handle_id = proto_handle,
    .type = NULL,
    .locked = false,
    .object = NULL,
    .owner = NULL
    };
}

status_t grab_table_space(IO_OBJ_TYPE type, int32_t* result){
    return 0; //TODO - don't do this
}
/** Public Functions **/

status_t IO_INIT(){
    for (int i = 0; i < IO_OBJ_COUNT; i++){
        filesystems[i] = NULL;
        devices[i] = NULL;
        mounts[i] = NULL;
        middlewares[i] = NULL;
        for (int j =0; j < 5; j++){
            messages[i + j] = NULL;
        }
    }

    for (int i = 0; i < 100; i++){
        HANDLE_TABLE[i] = NULL;
    }
    has_initted = true;
    return E_SUCCESS;
}

status_t IO_PROTOTYPE(IO_OBJ_TYPE type, PIOHANDLE handle){
    // Make sure we've set up
    if (has_initted == false){
        IO_INIT();
    }
    //grab a handle
    IO_HANDLE handle;
    status_t stat = grab_handle(*handle);
    if (stat != E_SUCCESS) {
        return stat;
    }
    //set up the object
    switch (type) {
        case IO_OBJ_FILESYSTEM:
            int32_t table_index;
            grab_table_space(IO_OBJ_FILESYSTEM, &table_index);
            //construct object
            filesystems[table_index] = {
                .handle = handle,
                .name = NULL,
                .created = NULL, //current time
                .execute = NULL,
                .init = NULL,
                .finalize = NULL
            };
            //update handle table
            HANDLE_TABLE[handle].type = IO_OBJ_FILESYSTEM;
            HANDLE_TABLE[handle].object = &filesystems[table_index];
            break;
        case IO_OBJ_DEVICE:
            //construct object
            //update handle table
            break;
        case IO_OBJ_MIDDLEWARE:
            //construct object
            //update handle table
            break;
        case IO_OBJ_MESSAGE:
            //construct object
            //update handle table
            break;
        case IO_OBJ_MOUNT:
            //construct object
            //update handle table
            break;
        default:
            return E_BAD_TYPE;
    }
    return E_NOT_IMPLEMENTED;
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
