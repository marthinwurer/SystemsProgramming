/**
 * \file pubfat.h
 * \author Nathan C. Castle
 * \description Public interface to the FAT32 Filesystem
 */
#include <kern/io/io_types.h>
#include <kern/io/message.h>
#include "clusters.h"
#include "create.h"
#include "delete.h"
#include "fat.h"
#include "meta.h"
#include "query.h"
#include "read.h"
#include "set.h"
#include "time.h"
#include "write.h"
#pragma once
/**
 * \brief Called by the I/O Manager to take action on a complete I/O Message
 * \param msg [in] pointer to message used to determine executed action
 * \param msg [out] pointer to message used for storing output
 */
status_t fat32_execute(PIO_MESSAGE msg);

/**
 * \brief initializes the filesystem as it is loaded by the I/O Manager
 */
status_t fat32_init();

/**
 * \brief called to destruct the filesystem, finishing any work and
 *        freeing any held resources
 */
status_t fat32_finalize();
