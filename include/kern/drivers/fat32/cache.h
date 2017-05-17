/**
 * \author Nathan C. Castle
 * \file cache.h
 * \brief definitions and data structures for caching
 */
#include "fat_types.h"
#pragma once

status_t init_fat_cache_disk(PFAT_CONTEXT ctx);
status_t invalidate_fat_cache(PFAT_CONTEXT ctx);
status_t flush_fat_cache(PFAT_CONTEXT ctx);
status_t read_cluster_entry(PFAT_CONTEXT ctx, int32_t id, int32_t* out); //get value for cluster in table
status_t write_cluster_entry(PFAT_CONTEXT ctx, int32_t id, int32_t value);
