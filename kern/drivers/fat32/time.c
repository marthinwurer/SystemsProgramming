/**
 * \brief implementation of time handling components for fat32 filesystem
 * \author Nathan C. Castle
 */
#include <kern/drivers/fat32/time.h>

FAT32TIME unix_to_fattime(int32_t timestamp) {
    //unix time is seconds from epoch
    //epoch starts at June 1, 1970
    FAT32TIME time = {
        .hour = 0,
        .minute = 0,
        .seconds = 0,
        .year = 1970,
        .month = 6, 
        .day = 1
    };
    return time;
}

int32_t fattime_to_unix (FAT32TIME timestamp) {
    return 0;
}

FAT32TIME fattime_from_parts(FAT_DATE_PACKED date, FAT_TIME_PACKED time) {
    FAT32TIME dt = {
        .hour = time.hour,
        .minute = time.minutes,
        .seconds = time.seconds,
        .year = date.year,
        .month = date.month,
        .day = date.day
    };
    return dt;
}

FAT32TIME fattime_from_date(FAT_DATE_PACKED date) {
    FAT32TIME time = {
        .hour = 0,
        .minute = 0,
        .seconds = 0,
        .year = date.year,
        .month = date.month,
        .day = date.day
    };
    return time;
}
