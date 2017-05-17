/**
 * \brief simple wrapper for installing mount points
 */
#include <kern/io/router.h>

/**
 * \brief installs a new mount
 * \param name [in] name for the mountpoint
 * \param path [in] path associated with the mount
 * \param device [in] handle to the device to associate with the mount
 * \param filesystem [in] handle to the filesystem to associate with the mount
 */
status_t install_mount(char* name, char* path, IOHANDLE device, IOHANDLE filesystem);

/**
 * \brief uninstalls the specified mount
 */
status_t uninstall_mount(IOHANDLE handle);
