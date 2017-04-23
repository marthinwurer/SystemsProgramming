#include "io_types.h"

typedef struct _io_message {
    IOHANDLE        handle;
    char*           path;
    void*           filesystem; //PIO_FILESYSTEM
    void*           device; //PIO_DEVICE
    char*           buffer;
    int32_t         length;
    status_t        status;
} IO_MESSAGE, *PIO_MESSAGE;

status_t _io_msg_setprop(PIO_MESSAGE msg, IOPROP prop, void* value, int32_t blength);

IO_MESSAGE _io_msg_init_null(void);

IO_MESSAGE _io_msg_init_handle(IOHANDLE handle);

status_t _io_init_messages(void);

status_t _io_grab_next_message(PIO_MESSAGE* out_p_message);

status_t _io_msg_iterate(PIOHANDLE out, int index);
