/**
 * @file: middleware.h
 * @author: Nathan C. Castle
 * @summary: Definitions related to middleware
 **/
#include <baseline/common.h>
#include "message.h"
#pragma once

typedef struct _io_middleware {
  char* name;
  status_t (*result)(p_io_message message);
} io_middleware, *p_io_middleware;
