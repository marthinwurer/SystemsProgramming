/**
 * \Author Nathan C. Castle
 * \brief RAW File System; operates on a block-by-block basis
 *        Could theoretically be used by higher-level tooling in the absence of a proper filesystem
 */
#include <kern/io/io_types.h>
#include <kern/io/router.h>
#include <libpath.h>
#pragma once

/**
 * \brief called to install the fs at boot
 */
status_t raw_install(void);

/**
 * \brief function called by the router upon a new mount point being installed
 */
status_t raw_init(void);

/**
 * \brief called by the io router to let the FS handle a message
 */
status_t raw_execute(PIO_MESSAGE msg);

/**
 * \brief function called by the IO router to destroy the FS and free its resources
 */
status_t raw_finalize(void);
