#include "io_types.h"

typedef struct _io_middleware {
    IOHANDLE        handle; ///< Handle associated with the middleware object
    char*           name; ///< Name associated with the middleware
    int32_t         created; ///< Datetime of creation
    status_t        (*execute)(void* msg); //Middleware function; takes PIO_MESSAGE
} IO_MIDDLEWARE, *PIO_MIDDLEWARE;

/**
 * \brief function for setting specified property of an IO_MIDDLEWARE object
 * \param md [in] Pointer to IO_MIDDLEWARE Object
 * \param prop [in] Property to set
 * \param value [in] Pointer to the value to be set
 * \param length [in] Length of data specified by @value
 */
status_t _io_md_setprop(PIO_MIDDLEWARE md, IOPROP prop, void* value, int32_t length);

/**
 * \brief function constructs a null IO_MIDDLEWARE object and returns it by value
 */
IO_MIDDLEWARE _io_md_init_null(void);

/**
 * \brief function constructs an empty IO_MIDDLEWARE object and returns it by value
 * \param handle Handle value to associate with constructed middleware object
 */
IO_MIDDLEWARE _io_md_init_handle(IOHANDLE handle);

/**
 * \brief Initializes static middleware data structures
 */
status_t _io_init_middlewares(void);

/**
 * \brief Gets the next available IO_MIDDLEWARE object for allocation
 * \param out_p_middleware Pointer to a Pointer to an IO_MIDDLEWARE; the IO_MIDDLEWARE Pointer is updated with the address of the grabbed middleware object.
 */
status_t _io_grab_next_middleware(PIO_MIDDLEWARE* out_p_middleware);

/**
 * \brief gets a handle for a middleware object by index
 * \param out [out] Pointer to a handle that is updated with the handle for the middleware at specified index
 * \param index index of middleware to return
 */
status_t _io_md_iterate(PIOHANDLE out, int index);

/**
 * \brief function for getting specified property of an IO_MIDDLEWARE object
 * \param md [in] Pointer to IO_MIDDLEWARE Object
 * \param prop [in] Property to get
 * \param value [out] Out pointer for returned value
 * \param plength [out] Length of data returned or available in case of E_MORE_DATA
 */
status_t _io_md_getprop(PIO_MIDDLEWARE md, IOPROP prop, void* value, PBSIZE plength);
