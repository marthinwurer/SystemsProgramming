#include "io_types.h"
#include <baseline/common.h>
#include <string.h>

typedef struct _io_device {
    IOHANDLE        handle;
    char*           name;
    int32_t         created;
    status_t        (*read)(int offset, PBSIZE length, void* buffer);
    status_t        (*write)(int offset, PBSIZE length, void* buffer);
    status_t        (*finalize)();
} IO_DEVICE, *PIO_DEVICE;

status_t _io_dv_setprop(PIO_DEVICE dv, IOPROP prop, void* value, int32_t length);

IO_DEVICE _io_dv_init_null(void);

IO_DEVICE _io_dv_init_handle(IOHANDLE handle);
