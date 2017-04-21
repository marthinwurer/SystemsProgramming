/**
 * @file: filesystem.h
 * @author: Nathan C. Castle
 * @summary: Definitions related to the Filesystem Extensibility Model
 */
#include <baseline/common.h>
#include "message.h"
#pragma once

typedef struct _io_filesystem {
  char* name;
  status_t (*read)(p_io_message message);
  status_t (*write)(p_io_message message);
  status_t (*query)(p_io_message message);
  status_t (*set)(p_io_message message);
  status_t (*reaccomodate)(p_io_message message);
} io_filesystem, *p_io_filesystem;
