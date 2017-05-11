/**
 * @file: io_types.h
 * @summary: common definition for types used by the I/O subsystem
 * @author: Nathan C. Castle
 * */
#pragma once
#include <string.h>
#include <baseline/common.h>
#include <baseline/prettyprinter.h>

#define ENABLEHANDLERS status_t stat_h = E_SUCCESS;

#define HANDLED(code) if((stat_h = code) != E_SUCCESS){ pretty_print(stat_h); return stat_h; }
#define IGNORED(code) code
#define RETURNED(code) return code

typedef int32_t IOHANDLE, *PIOHANDLE; //handle to I/O objects
typedef int32_t FILEHANDLE, *PFILEHANDLE; //handle to open files
typedef int32_t BSIZE, *PBSIZE, BOFFSET, *PBOFFSET; //buffer size, bytes
typedef int32_t IO_UNIX_TS, *PIO_UNIX_TS;

#define IOHANDLE_NULL (IOHANDLE)-1

typedef enum _ioctl {
    IOCTL_READ,
    IOCTL_APPEND,
    IOCTL_WRITE,
    IOCTL_EXPAND,
    IOCTL_DELETE,
    IOCTL_QUERY,
    IOCTL_CREATE,
    IOCTL_ENUMERATE,
    IOCTL_IDENTIFY
} IOCTL, *PIOCTL;

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
    IOPROP_IOCTL,
    IOPROP_BUFFER,
    IOPROP_PROP
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
