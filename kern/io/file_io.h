/**
 *
 * Public Definitions for FILE IO API
 * @author: Nathan C. Castle
 * @file: file_io.h
 * */

#include <baseline/common.h>
#include "io_types.h"

/**
 * @summary: Opens a connection to a file, returning a handle to that file
 * @returns: status_t
 * @in path: fully qualified path to file, including mount point
 * @out handle: pointer to handle
 * @post-condition: metadata is loaded and ready to be read
 * */
status_t IoOpenFile(char* path, PIOHANDLE handle);

/**
 * @summary: Reads a file
 * @returns: status_t
 *          E_SUCCESS if data read was succesful
 *          E_LESS_DATA if less data in file than size of buffer
 *          E_MORE_DATA if more data in file than size of buffer
 * @out: buffer - data read from file
 * @out: length - length of data in file
 * @in: length - length of data to read from the file; can be less than size of file
 * @in: handle - handle of file to read from
 * @in: offset - offset from current position in file to read
 * @out: offset - new file cursor position
 * */
status_t IoReadFile(IOHANDLE handle, PBSIZE offset, PBSIZE length, void* buffer);

/**
 * @summary: Writes content to a file
 * @returns: status_t
 *          E_SUCCESS if write was a success
 *          E_MORE_DATA if write exceeds length of file and expand_file is false
 * @in: handle - handle of file to write to
 * @in: offset - index into file
 * @in: length - length of data to write to file
 * @in: expand_file - file auto expands if write past end
 * @in: buffer - data to write to file
 * @out: offset - new file cursor position
 * */
status_t IoWriteFile(IOHANDLE handle, PBSIZE offset, PBSIZE length, bool expand_file, void* buffer);

/**
 * @summary: Advances the position of the cursor within a file
 * @returns: status_t
 *          E_SUCCESS if advance succeeds
 *          E_OUT_OF_BOUNDS if advance is out of bounds
 * @in: handle - file to advance
 * @in: auto_expand - file to expand automatically if size exceeded
 * @in: offset - amount to advance; can be negative to rewind
 * @out: new file cursor position
 * */
status_t IoAdvanceFile(IOHANDLE handle, int auto_expand, PBSIZE offset);

/**
 * @summary: Queries a metadata property of a file 
 * @returns status_t
 *          E_BAD_PROP if property selected is invalid
 *          E_MORE_DATA if provided buffer can't fit all data
 * @in handle - file to query
 * @in property - enum value; property to read
 * @out buffer - place to store data
 * @out length - length of data to read; size returned here
 **/
status_t IoQueryFile(IOHANDLE handle, IOPROP property, void* buffer, PBSIZE length);

/**
 * @summary: updates a file's property metadata
 * @returns status_t
 *          E_BAD_PROP if property selected is invalid
 *          E_IMMUTABLE if property is valid but can't be changed this way
 *          E_MORE_DATA if too much data was provided
 * @in handle - file to modify
 * @in property - property of file to modify
 * @in buffer - value to update property to
 * @in length - size of value to write
 * @out length - size of value that was written
 * */
status_t IoSetFile(IOHANDLE handle, IOPROP property, void* buffer, PBSIZE length);

/**
 * @summary: deletes a file
 * @returns status_t
 * @in handle - file to delete
 **/
status_t IoDeleteFile(IOHANDLE handle);

/**
 * @summary: gets a child of the current directory
 * @returns: status_t
 *          E_OUT_OF_BOUNDS - asked for child that didn't exist
 *          E_NOT_DIR - asked for children of a file
 *          E_MORE_DATA - length of name exceeded provided buffer
 *          E_LESS_DATA - length of name shorted than provided buffer
 * @in: IOHANDLE - directory to read from
 * @in: property to read
 * @in: offset - index of child to get data from
 * @out: data
 * @out: length - length of buffer/length of name
 * */
status_t IoEnumerateChild(IOHANDLE handle, IOPROP property, int child, void* buffer, PBSIZE length);
