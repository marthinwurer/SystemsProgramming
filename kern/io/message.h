/**
 * @file: message.h
 * @author: Nathan C. Castle
 * @summary: public headers for the message component of the i/o system.
 **/

#include <baseline/common.h>
#pragma once
typedef enum _io_ioctl {
  IO_CTL_READ,
  IO_CTL_WRITE,
  IO_CTL_APPEND,
  IO_CTL_QUERY,
  IO_CTL_SET,
  IO_CTL_REACCOMODATE //delete
} io_ioctl, *p_io_ioctl;

/**
 * Messages are used to encapsulate all of the state related to an I/O operation.
 * Messages are passed down through the stack, being processed by the router, filters and ultimately filesystem.
 */
typedef struct _io_message {
  char*           path;
  int32_t         offset;
  int32_t         buffer_size;
  int32_t         timestamp_open;
  int32_t         timestamp_close;
  void*           file_system; //p_io_filesystem
  void*           device; //p_io_device
  void*           input_buffer;
  void*           output_buffer;
  io_ioctl        ioctl;
  pid_t*          owner;
} io_message, *p_io_message;
