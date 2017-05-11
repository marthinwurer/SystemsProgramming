/**
 * temporary shim to work around malloc not being here
 */
#include <baseline/common.h>
#include <kern/memory/memory_map.h>
#pragma once
#define malloc malloc_io
#define free free_io
void* malloc_io(int32_t size);

int free_io(void* address);
