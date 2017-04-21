/**
 * @file: io_types.h
 * @summary: common definition for types used by the I/O subsystem
 * @author: Nathan C. Castle
 * */

typedef int IOHANDLE, *PIOHANDLE; //handle to an open file
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
    IOPROP_MOUNTED_FS, //filesystem for use with mounts
    IOPROP_MOUNTED_DEVICE, //device for use with mounts
    IOPROP_FS_EXEC,
    IOPROP_FS_INIT,
    IOPROP_FS_FINALIZE
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

