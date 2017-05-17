/**
 * \Author Nathan C. Castle
 * \brief implementation of an in-memory backing store for I/O
 */

#include <baseline/common.h>
#include <kern/io/io_types.h>
#pragma once
/**
 * \brief installs an instance of the ramdisk driver with the io router
 */
status_t ramdisk_install(void);

/**
 * \brief implements byte-based reading from memory
 * \param offset [in] offset from start of tape (bytes)
 * \param length [in] size of buffer to read into (bytes)
 * \param length [out] size of data read (bytes)
 * \param buffer [out] pointer to storage for read data
 */
status_t ramdisk_read(BOFFSET offset, PBSIZE length, void* buffer);

/**
 * \brief implements byte-based writing to memory
 * \param offset [in] offset from start of tape (bytes)
 * \param length [in] size of data to write (bytes)
 * \param length [out] size of data written (bytes)
 * \param buffer [in] pointer to data to write
 */
status_t ramdisk_write(BOFFSET offset, PBSIZE length, void* buffer);

/**
 * \brief function destructs ramdisk, freeing any held resources
 */
status_t ramdisk_finalize(void);
