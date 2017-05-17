#include "io_types.h"

typedef struct _io_filesystem {
    IOHANDLE        handle; ///< Handle associated with the filesystem
    char*           name; ///< Name associated with the filesystem
    status_t        (*execute)(void* msg); ///< Function for handling a message; takes a PIO_MESSAGE as input
    status_t        (*init)(); ///< Function for setting up the FS
    status_t        (*finalize)(); ///< Function for deconstructing the FS
} IO_FILESYSTEM, *PIO_FILESYSTEM;

/**
 * \brief function for setting specified property of an IO_FILESYSTEM object
 * \param fs [in] Pointer to IO_FILESYSTEM Object
 * \param prop [in] Property to set
 * \param value [in] Pointer to the value to be set
 * \param length [in] Length in bytes of data specified by @value
 */
status_t _io_fs_setprop(PIO_FILESYSTEM fs, IOPROP prop, void* value, BSIZE length);

/**
 * \brief function constructs a null IO_FILESYSTEM object and returns it by value
 */
IO_FILESYSTEM _io_fs_init_null(void);

/**
 * \brief function constructs an empty IO_FILESYSTEM object and returns it by value
 * \param handle Handle value to associate with constructed filesystem
 */
IO_FILESYSTEM _io_fs_init_handle(IOHANDLE handle);

/**
 * \brief Initializes static filesystem data structures
 */
status_t _io_init_filesystems(void);

/**
 * \brief Gets the next available IO_FILESYSTEM for allocation
 * \param out_p_fs Pointer to a Pointer to an IO_FILESYSTEM; the IO_FILESYSTEM Pointer is updated with the address of the grabbed filesystem.
 */
status_t _io_grab_next_filesystem(PIO_FILESYSTEM* out_p_fs);

/**
 * \brief gets a handle for a filesystem by index
 * \param out [out] Pointer to a handle that is updated with the handle for the filesystem at specified index
 * \param index index of filesystem to return
 */
status_t _io_fs_iterate(PIOHANDLE out, int index);

/**
 * \brief function for getting specified property of an IO_FILESYSTEM object
 * \param fs [in] Pointer to IO_FILESYSTEM Object
 * \param prop [in] Property to get
 * \param value [out] Pointer to the returned value
 * \param plength [out] Size of value in bytes returned or available in case of E_MORE_DATA
 */
status_t _io_fs_getprop(PIO_FILESYSTEM fs, IOPROP prop, void* value, PBSIZE plength);
