#include "io_types.h"

typedef struct _io_filesystem {
    IOHANDLE        handle;
    char*           name;
    int32_t         created;
    status_t        (*execute)(void* msg);//PIO_MESSAGE
    status_t        (*init)();
    status_t        (*finalize)();
} IO_FILESYSTEM, *PIO_FILESYSTEM;

status_t _io_fs_setprop(PIO_FILESYSTEM fs, IOPROP prop, void* value, int32_t length);

IO_FILESYSTEM _io_fs_init_null(void);

IO_FILESYSTEM _io_fs_init_handle(IOHANDLE handle);

status_t _io_init_filesystems(void);

status_t _io_grab_next_filesystem(PIO_FILESYSTEM* out_p_fs);

status_t _io_fs_iterate(PIOHANDLE out, int index);
