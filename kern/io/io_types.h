/**
 * @file: io_types.h
 * @summary: common definition for types used by the I/O subsystem
 * @author: Nathan C. Castle
 * */
#pragma once
#include <string.h>
#include <baseline/common.h>
typedef int IOHANDLE, *PIOHANDLE; //handle to I/O objects
typedef int FILEHANDLE, *PFILEHANDLE; //handle to open files
typedef int BSIZE, *PBSIZE; //buffer size, bytes

typedef enum _file_prop {
    IOPROP_IS_DIR,
    IOPROP_COUNT_CHILDREN,
    IOPROP_PARENT_PATH,
    IOPROP_PATH,
    IOPROP_LAST_READ,
    IOPROP_LAST_MODIFIED,
    IOPROP_CREATED,
    IOPROP_LENGTH_CONTENT,
    IOPROP_CURSOR_POSITION,
    IOPROP_NAME,
    IOPROP_FILESYSTEM, //filesystem for use with mounts
    IOPROP_DEVICE, //device for use with mounts
    IOPROP_EXECUTE,
    IOPROP_INIT,
    IOPROP_READ,
    IOPROP_WRITE,
    IOPROP_FINALIZE,
    IOPROP_BUFFER
} IOPROP, *PIOPROP;

typedef enum io_obj_type {
    IO_OBJ_FILESYSTEM,
    IO_OBJ_DEVICE,
    IO_OBJ_FILTER,
    IO_OBJ_FILE,
    IO_OBJ_MIDDLEWARE,
    IO_OBJ_MESSAGE,
    IO_OBJ_MOUNT,
    IO_OBJ_UNKNOWN
} IO_OBJ_TYPE, *P_IO_OBJ_TYPE;

typedef enum _io_sort {
    IO_SORT_ASC,
    IO_SORT_DSC,
    IO_SORT_RND
} IOSORT, *PIOSORT;

typedef enum _io_create_policy {
    IO_CP_CREATE,
    IO_CP_CREATERECURSIVE,
    IO_CP_FAIL
} IOCREATEPOLICY, *PIOCREATEPOLICY;
