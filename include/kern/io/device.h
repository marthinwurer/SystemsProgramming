#include "io_types.h"
#pragma once
typedef struct _io_device {
    IOHANDLE        handle; ///< Handle associated with the device
    char*           name; ///< Name associated with the device
    status_t        (*read)(int offset, PBSIZE length, void* buffer); ///< Pointer to function for reading bytes
    status_t        (*write)(int offset, PBSIZE length, void* buffer); ///< Pointer to function for writing bytes
    status_t        (*finalize)(); ///< Pointer to function for deconstructing the device
} IO_DEVICE, *PIO_DEVICE;

/**
 * \brief function for setting specified property of an IO DEVICE object
 * \param dv [in] Pointer to IO DEVICE Object
 * \param prop [in] Property to set
 * \param value [in] Pointer to the value to be set
 * \param length [in] Length of data specified by @value
 */
status_t _io_dv_setprop(PIO_DEVICE dv, IOPROP prop, void* value, int32_t length);

/**
 * \brief function constructs a null IO DEVICE object and returns it by value
 */
IO_DEVICE _io_dv_init_null(void);

/**
 * \brief function constructs an empty IO DEVICE object and returns it by value
 * \param handle Handle value to associate with constructed device
 */
IO_DEVICE _io_dv_init_handle(IOHANDLE handle);

/**
 * \brief Initializes static device tables
 */
status_t _io_init_devices(void);

/**
 * \brief Gets the next available IO Device for allocation
 * \param out_p_device Pointer to a Pointer to an IO Device; the IO Device Pointer is updated with the address of the grabbed device.
 */
status_t _io_grab_next_device(PIO_DEVICE* out_p_device);

/**
 * \brief gets a handle for a device by index
 * \param out [out] Pointer to a handle that is updated with the handle for the device at specified index
 * \param index index of device to return
 */
status_t _io_dv_iterate(PIOHANDLE out, int index);

/**
 * \brief function for getting specified property of an IO DEVICE object
 * \param dv [in] Pointer to IO DEVICE Object
 * \param prop [in] Property to get
 * \param value [out] Pointer for returned value
 * \param plength [out] Size of returned data or available data in case of E_MORE_DATA
 */
status_t _io_dv_getprop(PIO_DEVICE dv, IOPROP prop, void* value, PBSIZE plength);
