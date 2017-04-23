#include "io_types.h"

typedef struct _io_mount {
    IOHANDLE        handle;
    char*           name;
    char*           path;
    int32_t         created;
    IOHANDLE        filesystem;
    IOHANDLE        device;
} IO_MOUNT, *PIO_MOUNT;

status_t _io_mp_setprop(PIO_MOUNT mp, IOPROP prop, void* value, int32_t length);

IO_MOUNT _io_mp_init_null(void);

IO_MOUNT _io_mp_init_handle(IOHANDLE handle);

status_t _io_init_mounts(void);

status_t _io_grab_next_mount(PIO_MOUNT* out_p_mount);

status_t _io_mp_iterate(PIOHANDLE out, int index);
