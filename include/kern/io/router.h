/**
 * @file: router.h
 * @author: Nathan C. Castle
 * @summary: I/O Manager Public definitions
 **/
#include "io_types.h"
#include "filesystem.h"
#include "middleware.h"
#include "device.h"
#include "mount.h"
#include "message.h"
#pragma once

/**
 * \brief handles initialization of the IO subystem
 */
status_t IO_INIT();
/**
 * \brief allocates space for an object and returns handle
 * \return status_t: E_SUCCESS or E_NO_MORE_HANDLES
 * \param obj_type [in] type of object to prototype
 * \param  phandle [in] object handle
 */
status_t IO_PROTOTYPE(IO_OBJ_TYPE type, PIOHANDLE handle);

/**
 * \brief updates a property of an IO object
 * \return status_t: E_SUCCESS, E_BAD_HANDLE, E_BAD_PROPERTY,
 *          E_INVALID_VALUE, E_LESS_DATA, E_LOCKED
 * \param handle [in] handle to object being updated
 * \param property [in] property to update
 * \param value [in] value to set
 * \param length [in] length of value to set [out] length of value written
 * */
status_t IO_UPDATE(IOHANDLE handle, IOPROP property, void* value, PBSIZE length);

/*
 * wrapper around IO_UPDATE for int32_ts
 */
status_t IO_UPDATE_VALINT(IOHANDLE handle, IOPROP property, int32_t value);

/*
 * wrapper around IO_UPDATE for IOCTLs
 */
status_t IO_UPDATE_IOCTL(IOHANDLE handle, IOCTL value);

/*
 * wrapper around IO_UPDATE for raw pointer values
 * Not for use with buffers that need a reliable length value
 */
status_t IO_UPDATE_VOID(IOHANDLE handle, IOPROP property, void* value);

/**
 * wrapper around IO_UPDATE for use with strings
 */
status_t IO_UPDATE_STR(IOHANDLE handle, IOPROP property, char* value);

/**
 * \brief deletes an object; interpreted as cancel for IO Messages
 * \return status_t: E_SUCCESS or E_BAD_HANDLE
 * \param handle [in] object to delete
 * */
status_t IO_DELETE(IOHANDLE handle);

/**
 * \brief for use with IO Message; executes IO pipeline
 * \return status_t: E_SUCCESS or E_BAD_HANDLE
 * \param handle [in] handle to IO Message representing request
 * */
status_t IO_EXECUTE(IOHANDLE handle);

/**
 * \brief enumerates through the objects of a particular type
 * \return status_t: E_SUCCESS or E_OUT_OF_BOUNDS
 * \param type [in] type of object to iterate for
 * \param index [in] index of object (in view of iteration)
 * \param object [out] handle to object that was found
 * */
status_t IO_ENUMERATE(IO_OBJ_TYPE type, int index, PIOHANDLE object);

/**
 * \brief locks an object from further modification
 * \return status_t: E_SUCCESS, E_BAD_HANDLE, or E_NOT_LOCKABLE
 * \param handle [in] handle to object to be locked
 * */
status_t IO_LOCK(IOHANDLE handle);

/**
 * \brief reads a property of an IO object by handle
 * \param handle [in] handle for which to read property
 * \param prop [in] property to read
 * \param buffer [out] buffer in which to write result data
 * \param plength [in] size of buffer to write to. [out] actual size of data
 */
status_t IO_INTERROGATE(IOHANDLE handle, IOPROP prop, void* buffer, PBSIZE plength);
