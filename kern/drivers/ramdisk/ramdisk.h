/**
 * \Author Nathan C. Castle
 * \brief implementation of an in-memory backing store for I/O
 */

#include <baseline/common.h>
#include "../../io/io_types.h"

/**
 * \brief installs an instance of the ramdisk driver with the io router
 */
status_t ramdisk_install(void);

/**
 * \brief implements block-based reading from memory
 * \param offset [in] offset from start of tape (blocks)
 * \param length [in] size of buffer to read into (blocks)
 * \param length [out] size of data read (blocks)
 * \param buffer [out] pointer to storage for read data
 */
status_t ramdisk_read(int offset, PBSIZE length, void* buffer);

/**
 * \brief implements block-based writing to memory
 * \param offset [in] offset from start of tape (blocks)
 * \param length [in] size of data to write (blocks)
 * \param length [out] size of data written (blocks)
 * \param buffer [in] pointer to data to write
 */
status_t ramdisk_write(int offset, PBSIZE length, void* buffer);

/**
 * \brief function destructs ramdisk, freeing any held resources
 */
status_t ramdisk_finalize(void);
