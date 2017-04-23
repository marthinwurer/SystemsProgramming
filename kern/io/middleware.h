#include "io_types.h"

typedef struct _io_middleware {
    IOHANDLE        handle;
    char*           name;
    int32_t         created;
    status_t        (*execute)(void* msg); //PIO_MESSAGE
} IO_MIDDLEWARE, *PIO_MIDDLEWARE;

status_t _io_md_setprop(PIO_MIDDLEWARE md, IOPROP prop, void* value, int32_t length);

IO_MIDDLEWARE _io_md_init_null(void);

IO_MIDDLEWARE _io_md_init_handle(IOHANDLE handle);

status_t _io_init_middlewares(void);

status_t _io_grab_next_middleware(PIO_MIDDLEWARE* out_p_middleware);

status_t _io_md_iterate(PIOHANDLE out, int index);
