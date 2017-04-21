/**
 * @file: router.h
 * @author: Nathan C. Castle
 * @summary: I/O Manager Public definitions
 **/
#include <baseline/common.h>
#include "io_types.h"

/**
 * @summary: allocates space for an object and returns handle
 * @returns status_t
 *          E_SUCCESS
 *          E_NO_MORE_HANDLES
 * @in obj_type - type of object to prototype
 * @out phandle - object handle
 * */
status_t IO_PROTOTYPE(IO_OBJ_TYPE type, PIOHANDLE handle);

/**
 * @summary: updates a property of an IO object
 * @returns status_t
 *          E_SUCCESS
 *          E_BAD_HANDLE
 *          E_BAD_PROPERTY
 *          E_INVALID_VALUE
 *          E_LESS_DATA - less data than specified in length
 *          E_LOCKED - object has been locked and can't be updated
 * @in: handle - handle to object being updated
 * @in: property - property to update
 * @in: value - value to set
 * @in: length - length of value to set
 * @out: length - length of value written
 * */
status_t IO_UPDATE(IOHANDLE handle, IOPROP property, void* value, PBSIZE length);

/**
 * @summary: deletes an object; interpreted as cancel for IO Messages
 * @returns status_t
 *          E_SUCCESS
 *          E_BAD_HANDLE
 * @in: handle - object to delete
 * */
status_t IO_DELETE(IOHANDLE handle);

/**
 * @summary: for use with IO Message; executes IO pipeline
 * @returns status_t
 *          E_SUCCESS
 *          E_BAD_HANDLE
 *          **other IO pipeline results here **
 * @in: handle
 * */
status_t IO_EXECUTE(IOHANDLE handle);

/**
 * @summary: enumerates through the objects of a particular type
 * @returns status_t
 *          E_SUCCESS
 *          E_OUT_OF_BOUNDS
 * @in: type of object to iterate for
 * @in: index of object (in view of iteration)
 * @out: object - handle to object that was found
 * */
status_t IO_ENUMERATE(IO_OBJ_TYPE type, int index, PIOHANDLE object);

/**
 * @summary: locks an object from further modification
 * @returns status_t
 *          E_SUCCESS
 *          E_BAD_HANDLE
 *          E_NOT_LOCKABLE
 * @in: handle
 * */
status_t IO_LOCK(IOHANDLE handle);
