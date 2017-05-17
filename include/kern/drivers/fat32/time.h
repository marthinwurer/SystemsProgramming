/**
 * \author Nathan C. Castle
 * \brief functions and types useful for manipulating time
 */
#include <baseline/common.h>
#include <kern/drivers/fat32/fat_types.h>
#pragma once

FAT32TIME unix_to_fattime (int32_t timestamp);

int32_t fattime_to_unix (FAT32TIME timestamp);

FAT32TIME fattime_from_parts (FAT_DATE_PACKED date, FAT_TIME_PACKED time);

FAT32TIME fattime_from_date (FAT_DATE_PACKED date);
