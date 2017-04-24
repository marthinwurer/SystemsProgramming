/**
 * \author Nathan C. Castle
 * \file file.h
 * \brief Definition of the File-Oriented I/O API.
 *        This is intended for consumption by user-mode apps
 */

#include <baseline/common.h>
#include "../io/io_types.h"

/**
 * \brief opens a file by path
 * \param path [in] path to the file being opened. Must be fully qualified path starting at root.
 * \param strat [in] defines the policy for handling situations where the path points to a 
 *        nonexisting file. (e.g. create, create recursive, fail)
 * \param out [out] If successful, the provided IOHANDLE is updated with a handle to the opened
 *        or created file.
 */
status_t IoFileOpen (char* path, IOCREATEPOLICY strat, PIOHANDLE out);

/**
 * \brief Reads file contents into the provided output buffer
 * \pre IOHANDLE file must be a valid handle returned by IoFileOpen
 * \param file [in] identifies the file to read from
 * \param offset [in] defines the byte offset from the current file cursor
 *        read starts from position cursor + offset
 * \post file cursor is set to cursor + offset + plength
 * \param plength [in] max number of bytes to read. [out] number of bytes read
 * \param out [out] buffer in which read data is stored
 */
status_t IoFileRead (IOHANDLE file, BSIZE offset, PBSIZE plength, void* out);

/**
 * \brief Writes the contents of a buffer to a file
 * \pre IOHANDLE file must be a valid handle returned by IoFileOpen
 * \param file [in] identifies file to be written to
 * \param offset [in] defines byte offset from the current file cursor.
 *        Write starts at cursor + offset
 * \post file cursor is set to cursor + offset + plength
 * \param plength [in] size of input buffer in bytes. [out] number of bytes successfully written.
 * \param in [in] data buffer to be written
 */
status_t IoFileWrite (IOHANDLE file, BSIZE offset, PBSIZE plength, void* in);

/**
 * \brief Advances (or rewinds) file cursor
 * \pre IOHANDLE file must be a valid handle returned by IoFileOpen
 * \param file [in] identifies the file whose cursor is to be advanced
 * \param offset [in] byte offset to adjust cursor by
 * \post cursor = cursor + offset
 * \param poffset [out] new file cursor value
 */
status_t IoFileSeek (IOHANDLE file, BSIZE offset, PBSIZE poffset);

/**
 * \brief Reads a file's metadata
 * \pre IOHANDLE file must be a valid handle returned by IoFileOpen
 * \param file [in] identifies the file whose metadata is to be read
 * \param property [in] identifies the metadata property to read
 * \param plength [in] byte size of output buffer. [out] number of bytes in answer
 * \param out [out] output buffer; not updated if buffer length is less than bytes in answer
 */
status_t IoFileQuery (IOHANDLE file, IOPROP property, PBSIZE plength, void* out);

/**
 * \brief Writes a file's metadata
 * \pre IOHANDLE file must be a valid handle returned by IoFileOpen
 * \param file [in] identifies the file whose metadata is to be written
 * \param property [in] identifies the property to write
 * \param plength [in] byte size of the input buffer. [out] Number of bytes written.
 * \param in [in] input buffer
 */
status_t IoFileSet (IOHANDLE file, IOPROP property, PBSIZE plength, void* in);

/**
 * \brief deletes a file
 * \pre IOHANDLE file must be a valid handle returned by IoFileOpen
 * \param file identifies file to delete
 */
status_t IoFileDelete (IOHANDLE file);

/**
 * \brief Gets the child of a directory by index; supports sorting
 * \pre IOHANDLE file must be a valid handle returned by IoFileOpen
 * \param file [in] identifies the parent for whom children will be returned
 * \param property [in] identifies the property to sort by
 * \param sort [in] identifies the sort direction (e.g. ascending vs descending)
 * \param index [in] identifies the index of the child to return
 * \param plength [in] defines size of output buffer in bytes. [out] number of bytes in answer
 * \param out [out] string path to file; null if plength is less than the size of the path.
 */
status_t IoFileNextChild (IOHANDLE file, IOPROP property, IOSORT sort, int index, PBSIZE plength, char* out);

/**
 * \brief closes a file
 * \param file File to close
 * \post file handle is invalidated and made available for reuse. Callers should discard it without delay.
 */
status_t IoFileClose (IOHANDLE file);
