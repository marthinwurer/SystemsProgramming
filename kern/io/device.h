/**
 * @author: Nathan C. Castle
 * @file: device.h
 * @summary: Definitions related to the I/O Driver Model
 **/

#include <baseline/common.h>
#pragma once

typedef struct _io_device {
  status_t (*io_read)(int, int);//offset, run //pointer to device driver for 
  status_t (*io_write)(int , int , void* );//offset,run,buffer
} io_device, *p_io_device;
