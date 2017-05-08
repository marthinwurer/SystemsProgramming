#include "io_types.h"
#pragma once
typedef struct _io_message {
    IOHANDLE        handle; ///< Handle associated with the message
    char*           path; ///< Path for the message
    void*           filesystem; ///< Reference to associated filesystem; PIO_FILESYSTEM
    void*           device; ///< Reference to associated device; PIO_DEVICE
    char*           buffer; ///< Data buffer for storing input/output data
    int32_t         length; ///< Length of buffer
    status_t        status; ///< Current status of message; intended to support cancelation by middleware, etc.
    IOCTL           ioctl; ///< Operation to perform
    union POSITION {
        IOPROP property;
        int32_t offset;
    } offset_prop; ///< Controls where in file to perform operation, or specifies property to read/set
} IO_MESSAGE, *PIO_MESSAGE;

/**
 * \brief function for setting specified property of an IO_MESSAGE object
 * \param msg [in] Pointer to IO_MESSAGE Object
 * \param prop [in] Property to set
 * \param value [in] Pointer to the value to be set
 * \param length [in] Length of data specified by @value
 */
status_t _io_msg_setprop(PIO_MESSAGE msg, IOPROP prop, void* value, int32_t blength);

/**
 * \brief function constructs a null IO_MESSAGE object and returns it by value
 */
IO_MESSAGE _io_msg_init_null(void);

/**
 * \brief function constructs an empty IO_MESSAGE object and returns it by value
 * \param handle Handle value to associate with constructed message
 */
IO_MESSAGE _io_msg_init_handle(IOHANDLE handle);

/**
 * \brief Initializes static message data structures
 */
status_t _io_init_messages(void);

/**
 * \brief Gets the next available IO_MESSAGE for allocation
 * \param out_p_message Pointer to a Pointer to an IO_MESSAGE; the IO_MESSAGE Pointer is updated with the address of the grabbed message.
 */
status_t _io_grab_next_message(PIO_MESSAGE* out_p_message);

/**
 * \brief gets a handle for a message by index
 * \param out [out] Pointer to a handle that is updated with the handle for the message at specified index
 * \param index index of message to return
 */
status_t _io_msg_iterate(PIOHANDLE out, int index);

/**
 * \brief function for getting specified property of an IO_MESSAGE object
 * \param msg [in] Pointer to IO_MESSAGE Object
 * \param prop [in] Property to get
 * \param value [out] Out pointer for returned value
 * \param plength [out] Length of data returned or available in case of E_MORE_DATA
 */
status_t _io_msg_getprop(PIO_MESSAGE msg, IOPROP prop, void* value, PBSIZE plength);
