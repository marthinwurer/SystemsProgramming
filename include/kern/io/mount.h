#include "io_types.h"

typedef struct _io_mount {
    IOHANDLE        handle; ///< Handle associated with the mount point
    char*           name; ///< Name associated with the mount point
    char*           path; ///< Path of the mount point
    void*           fs_grab_bag; ///< generic storage for use by filesystem
    IOHANDLE        filesystem; ///< Handle referring to a filesystem
    IOHANDLE        device; ///< Handle referring to a device
} IO_MOUNT, *PIO_MOUNT;

/**
 * \brief function for setting specified property of an IO_MOUNT object
 * \param mp [in] Pointer to IO_MOUNT Object
 * \param prop [in] Property to set
 * \param value [in] Pointer to the value to be set
 * \param length [in] Length in bytes of data specified by @value
 */
status_t _io_mp_setprop(PIO_MOUNT mp, IOPROP prop, void* value, BSIZE length);

/**
 * \brief function constructs a null IO_MOUNT object and returns it by value
 */
IO_MOUNT _io_mp_init_null(void);

/**
 * \brief function constructs an empty IO_MOUNT object and returns it by value
 * \param handle Handle value to associate with constructed mount point
 */
IO_MOUNT _io_mp_init_handle(IOHANDLE handle);

/**
 * \brief Initializes static mount data structures
 */
status_t _io_init_mounts(void);

/**
 * \brief Gets the next available IO_MOUNT for allocation
 * \param out_p_mount Pointer to a Pointer to an IO_MOUNT; the IO_MOUNT Pointer is updated with the address of the grabbed mount point.
 */
status_t _io_grab_next_mount(PIO_MOUNT* out_p_mount);

/**
 * \brief gets a handle for a mountpoint by index
 * \param out [out] Pointer to a handle that is updated with the handle for the mount at specified index
 * \param index index of mount point to return
 */
status_t _io_mp_iterate(PIOHANDLE out, int index);

/**
 * \brief function for getting specified property of an IO_MOUNT object
 * \param mp [in] Pointer to IO_MOUNT Object
 * \param prop [in] Property to get
 * \param value [out] Pointer for storing out value
 * \param plength [out] Length in bytes of data returned or available in case of E_MORE_DATA
 */
status_t _io_mp_getprop(PIO_MOUNT mp, IOPROP prop, void* value, PBSIZE plength);
