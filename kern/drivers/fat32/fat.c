/**
 * \file fat.c
 * \author Nathan C. Castle
 * \brief implements several support functions for the FAT32 File System
 */
#include <kern/drivers/fat32/fat.h>
#include <kern/io/device.h>
#include <libpath.h>
/**
 * Calculation: 
 *      first sector: BIOS boot entry: 512 bytes
 *      second sector: FAT extended info: 512 bytes
 *      FAT 1: 32 bits * clusters_per_fat
 *      FAT 2: 32 bits * clusters_per_fat
 *      cluster #-2 * sizeof(cluster) //cluster is 32kb
 *
 * Special case:
 *      cluster entries 0 and 1 do not correspond to any area on disk
 *      these are reserved special values
 */
status_t byte_offset_for_cluster(PFAT_CONTEXT ctx, int32_t cluster, int32_t* out) {
    int32_t offset = 1024; //account for first two sectors
    offset += (4 * ctx->clusters_per_fat) * 2; //two FAT tables
    offset += cluster * ctx->cluster_size;
    *out = offset;
    return E_SUCCESS;
}

/**
 * first two sectors (512 bytes) are metadata
 * FAT1 starts immediately after metdata
 * FAT2 starts immediately after FAT1
 * size of each FAT is 4*clusters_per_fat bytes
 */
status_t byte_offset_fat(PFAT_CONTEXT ctx, int32_t fat_select, int32_t* out) {
    int32_t offset = 1024; //first two sectors (boot/bios)
    if (fat_select == 0) { 
        *out = offset; 
    } else {
        offset += 4 * ctx->clusters_per_fat;
        *out = offset;
    }
    return E_SUCCESS;
}

FAT_TIME_PACKED time_from_bytes(char b1, char b2) {
    return (FAT_TIME_PACKED){
        .hour = b1 >>3, 
            .minutes = b1 << 3 | b2 >> 5,
            .seconds = b2 << 3
    };
}

FAT_DATE_PACKED date_from_bytes(char b1, char b2) {
    return (FAT_DATE_PACKED) {
        .year = b1 >> 1,
            .month = ((b1 << 4) & 0x8) | b2 >> 5,
            .day = b2 & 0xF
    };
}

//starts at a position on disk (ideally the start of a directory entry)
//and reads it, building up a FAT_DENTRY and ultimately returning byte offset 
//of the next block entry in the table.
//unhandled case: long file name split over cluster boundary
FAT_DENTRY parse_dirtable_entry(PFAT_CONTEXT ctx, int32_t start, int32_t* end, int32_t dir_cluster) {
    //read entry into buffer. entry is 32 bytes
    char filename [255]; 
    int32_t start_start = start;
    int32_t highest_name_char;
    while (start - start_start < ctx->cluster_size) { //sanity check... don't read past cluster
        int32_t delength = 32;
        char* debuffer = malloc(32);
        status_t stat = ((PIO_DEVICE)(ctx->driver))->read(start, &delength, &debuffer);
        if (delength != 32) { return (FAT_DENTRY){}; }
        //is this a name entry or a file/dir entry?
        if (debuffer[11] == 0x0f) {
            //note: names are stored in UCS-2 format; basically UTF-16 but every char has 2 bytes
            //we will read the lower byte in each UCS-2 character as a single ascii char
            //also note: max of 256 characters in a name
            //this is a name block, append to name
            short position = debuffer[0];
            for (int i = 0; i < 5; i++) {
                filename[position*12+i] = debuffer[2 + 2*i];
            }
            for (int i = 0; i < 6; i++) {
                filename[position*12+i+5] = debuffer[14 + 2*i];
            }
            //useful when we need to copy string later
            if (position*12 + 12 > highest_name_char) { highest_name_char = position*12 + 12; }
        } else {
            //this is a file block, we're done
            if (debuffer[0] == 0) {
                *end = start + 32;
                return (FAT_DENTRY){};
                //end of table reached, return error
            } else if (debuffer[0] == 0xe5) {
                //unused entry, return error
                *end = start + 32;
                return (FAT_DENTRY){};
            } else if (debuffer[0] == 0x1) {
                //valid entry
                *end = start + 32; 
                //get attributes
                FAT_DE_ATTRIBUTE t =(FAT_DE_ATTRIBUTE)debuffer[1];
                //get times
                int32_t creation_secs = debuffer[13];
                FAT_TIME_PACKED created_time = time_from_bytes(debuffer[14], debuffer[15]);
                FAT_DATE_PACKED created_date = date_from_bytes(debuffer[16], debuffer[17]);
                FAT_DATE_PACKED read_date = date_from_bytes(debuffer[18], debuffer[19]);
                FAT_DATE_PACKED modified_date = date_from_bytes(debuffer[24], debuffer[25]);
                FAT_TIME_PACKED modified_time = time_from_bytes(debuffer[22], debuffer[23]);
                int32_t filesize = (debuffer[28] << 24) | (debuffer[29] << 16) | (debuffer[30] << 8) | debuffer[30];
                int32_t cluster = (debuffer[20] << 24) | (debuffer[21] << 16) | (debuffer[26] << 8) | debuffer[27];
                char* name = malloc(sizeof(char) * highest_name_char + 1);
                memcpy(name, filename, highest_name_char); //copy name
                return (FAT_DENTRY) {
                    .name = name,
                        .created = fattime_to_unix(fattime_from_parts(created_date, created_time)),
                        .read = fattime_to_unix(fattime_from_date(read_date)),
                        .updated = fattime_to_unix(fattime_from_parts(modified_date, modified_time)),
                        .filesize = filesize,
                        .cluster_start = cluster,
                        .child_count = -1,
                        .cluster_parent_de = dir_cluster,
                        .attr = t
                };
            }
        }
        start += delength;
    }
}

int32_t cluster_num_for_byte_offset(PFAT_CONTEXT ctx, int32_t offset) {
    //subtract offset from start of data
    //divide by cluster size
    int32_t offset_data = 0;
    byte_offset_for_cluster(ctx, 0, &offset_data);
    int32_t newoffset = offset - offset_data;
    return newoffset /= ctx->cluster_size;
}

FAT_ENTRY classify_clust(PFAT_CONTEXT ctx, int32_t cluster, int32_t* next) {
    int32_t offset_fat = 0; byte_offset_fat(ctx, 0, &offset_fat);
    int32_t size_entry = 4; 
    char* buffer = malloc(size_entry);
    offset_fat += 4 * cluster; //32bit entries == 4 bytes
    ((PIO_DEVICE)(ctx->driver))->read(offset_fat, &size_entry, &buffer);
    if (buffer[0] == 0 && buffer[1] == 0 && buffer[2] == 0 && buffer[3] == 0) {
        return FAT_CLUST_FREE;
    } else if (buffer[0] == 0 && buffer[1] == 0 && buffer[2] == 0 && buffer[3] == 1) {
        return FAT_CLUST_RESERVED;
    } else if (buffer[3] & 2 == 2) {
        *next = 99; //TODO - actually do this
        return FAT_CLUST_POINTER;
    } else if (buffer[3] >= 0xF8 && buffer[3] <= 0xFF) {
        return FAT_CLUST_LAST;
    } else {
        return FAT_CLUST_BAD;
    }
}

/** 
 * pre-condition: start is pointing at the head of a directory table
 */
status_t traverse_and_find_file_rec(PFAT_CONTEXT ctx, int32_t start, char* path, FAT_DENTRY* out) {
    //base case: empty path (shouldn't happen, return err)
    //pull out first part of path
    char* pathhead = io_path_head(path); //note: need to free
    char* pathtail = io_path_head(path);
    //scan through directory, examining pathnames
    int32_t newstart = start;
    int32_t current_clust = cluster_num_for_byte_offset(ctx, newstart);
    FAT_DENTRY file = parse_dirtable_entry(ctx, newstart, &newstart, current_clust);
    while (file.attr != NULL) {
        if (io_path_canonicalized_compare(pathhead, file.name) == 0) {
            if (file.attr & 0x10 == FAT_DE_DIRECTORY && pathtail != NULL){ 
                return traverse_and_find_file_rec(ctx, newstart, pathtail, out);
            } else if (file.attr & 0x10 != FAT_DE_DIRECTORY && pathtail == NULL) {
                *out = file;
                return E_SUCCESS;
            }
            return E_NO_MATCH;
        }
        //correct byte offset for next iter if necessary:
        if (newstart - start > ctx->cluster_size) {
            //find current cluster
            current_clust = cluster_num_for_byte_offset(ctx, newstart);
            //look up entry in fat
            int32_t next_clust = 0;
            FAT_ENTRY type = classify_clust(ctx, current_clust, &next_clust);
            //if entry is end of block, return file not found
            //if entry is pointer, newstart = byteoffset for newblock
            if (type == FAT_CLUST_POINTER) {
                start = next_clust;
                newstart = next_clust;
            } else {
                return E_NO_MATCH;
            }
        }
        file = parse_dirtable_entry(ctx, newstart, &newstart, current_clust);
    }
    //scanned through table, no match
    return E_NO_MATCH;
}

status_t find_file(PFAT_CONTEXT ctx, char* path, FAT_DENTRY* out) {
    //jump to root directory
    int32_t start = 0;
    byte_offset_for_cluster(ctx, 2, &start); //root is going to always start at clust 2
    return traverse_and_find_file_rec(ctx, start, path, out);
}

status_t read_next_dentry(PFAT_CONTEXT ctx, FAT_DENTRY* parent, int32_t offset, FAT_DENTRY* dentry) {
    return E_NOT_IMPLEMENTED;
}