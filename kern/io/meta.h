/**
 * @file: meta.h
 * @author: Nathan C. Castle
 * @summary: Definitions related metadata blocks
 **/
#pragma once

typedef struct _io_metadata {
  char* path;
  int32_t timestamp_read;
  int32_t timestamp_write;
  int32_t timestamp_create;
  int32_t size_bytes;
  int32_t permission_field;
} io_meta, *p_io_meta;
