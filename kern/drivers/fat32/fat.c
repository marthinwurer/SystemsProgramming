/**
 * \file fat.c
 * \author Nathan C. Castle
 * \brief implements several support functions for the FAT32 File System
 */
#include <kern/drivers/fat32/fat.h>
#include <kern/io/device.h>
#include <libpath.h>
#include <kern/drivers/fat32/time.h>

status_t traverse_and_find_file_rec_r(PFAT_CONTEXT ctx, int32_t* start, char* path, FAT_DENTRY* out);

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

//hour: 5bits, minute: 6bits, seconds: 5bits
void write_fat32_time(PFAT_CONTEXT ctx, FAT32TIME time, int32_t starting_offset) {
    char bytes[2];
    bytes[0] = time.hour << 3;
    bytes[0] = bytes[0] | ((time.minute >> 3)  & 0x7);
    bytes[1] = time.minute << 5;
    bytes[1] = bytes[1] | (time.seconds & 0x1F);
    int32_t size = 2;
    ((PIO_DEVICE)ctx->driver)->write(starting_offset, &size, &bytes);
}
//year: 7 bits. Month: 4 bits. Day: 5 bits
void write_fat32_date(PFAT_CONTEXT ctx, FAT32TIME date, int32_t starting_offset) {
    char bytes[2];
    bytes[0] = date.year << 1;
    bytes[0] = bytes[0] | ((date.month >> 3) & 0x1);
    bytes[1] = date.month << 4;
    bytes[1] = bytes[1] | (date.day & 0x1F);
    int32_t size = 2;
    ((PIO_DEVICE)ctx->driver)->write(starting_offset, &size, &bytes);
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
        ((PIO_DEVICE)(ctx->driver))->read(start, &delength, &debuffer);
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
            } else if (debuffer[0] == 0xe && debuffer[1] == 0x5) {
                //unused entry, return error
                *end = start + 32;
                return (FAT_DENTRY){};
            } else if (debuffer[0] == 0x1) {
                //valid entry
                *end = start + 32; 
                //get attributes
                FAT_DE_ATTRIBUTE t =(FAT_DE_ATTRIBUTE)debuffer[1];
                //get times
                //int32_t creation_secs = debuffer[13]; ignoring for now
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
    return (FAT_DENTRY){};
}

status_t free_cluster_chain(PFAT_CONTEXT ctx, int32_t cluster_start) {
    //find first cluster
    int32_t byte_offset;
    byte_offset_fat(ctx, 0, &byte_offset); //gets to the fat
    byte_offset += cluster_start * 4; //4 bytes per cluster
    
    //read in data
    char buffer[4];
    int32_t size = 4;
    int32_t next = 0;
    FAT_ENTRY entry = FAT_CLUST_POINTER;
    while (entry != FAT_CLUST_BAD && entry != FAT_CLUST_FREE && entry != FAT_CLUST_RESERVED) {
        int32_t value = buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3];
        ((PIO_DEVICE)ctx->driver)->read(byte_offset, &size, buffer);
        entry = classify_fat_entry(ctx, value, &next);
        buffer[0] = 0x0; buffer[1] = 0x0; buffer[2] = 0x0; buffer[3] = 0x0;
        ((PIO_DEVICE)ctx->driver)->write(byte_offset, &size, buffer);
        if (entry == FAT_CLUST_LAST) {
            return E_SUCCESS;
        }
        //extract cluster number
        int32_t cluster_num = (next >> 4) & 0xFFFFFFF;
        //then look up offset in table for cluster
        byte_offset_fat(ctx, 0, &byte_offset);
        byte_offset += 4 * cluster_num;
    }
    return E_SUCCESS;
}

//TODO - handle case where file doesn't already exist
status_t update_dentry(PFAT_CONTEXT ctx, int32_t parent_dir_cluster, FAT_DENTRY updated) {
    //read in existing d-entry
    int32_t byte_offset; //get to parent directory
    status_t stat_h = byte_offset_for_cluster(ctx, parent_dir_cluster, &byte_offset);
    //call parse_dir until we get the matching d-entry
    FAT_DENTRY old;
    HANDLED(traverse_and_find_file_rec_r(ctx, &byte_offset, updated.name, &old));
    FAT32TIME newtime;
    //go through and compare changes
    if (old.created != updated.created) {
        //generate FAT-style time
        newtime = unix_to_fattime(updated.created);
        write_fat32_time(ctx, newtime, byte_offset + 14);
        write_fat32_date(ctx, newtime, byte_offset + 16);
        //write changes as appropriate
    }
    if (old.read != updated.read){
        //generate FAT-style time
        newtime = unix_to_fattime(updated.read);
        write_fat32_date(ctx, newtime, byte_offset + 18);
        //write changes
    }
    if (old.updated != updated.updated) {
        //generate FAT-style time
        newtime = unix_to_fattime(updated.updated);
        write_fat32_time(ctx, newtime, byte_offset + 22);
        write_fat32_date(ctx, newtime, byte_offset + 24);
        //write changes
    }
    if (old.filesize != updated.filesize) {
        int32_t size = 4;
        HANDLED(((PIO_DEVICE)ctx->driver)->write(byte_offset + 28, &size, &updated.filesize)); 
    }
    if (old.attr != updated.attr) {
        char flags = (char) updated.attr;
        int32_t size = 1;
        HANDLED(((PIO_DEVICE)ctx->driver)->write(byte_offset + 1, &size, &flags));
    }
    return E_SUCCESS;
}

int32_t cluster_num_for_byte_offset(PFAT_CONTEXT ctx, int32_t offset) {
    //subtract offset from start of data
    //divide by cluster size
    int32_t offset_data = 0;
    byte_offset_for_cluster(ctx, 0, &offset_data);
    int32_t newoffset = offset - offset_data;
    return newoffset /= ctx->cluster_size;
}

FAT_ENTRY classify_fat_entry(PFAT_CONTEXT ctx, int32_t cluster, int32_t* next) {
    int32_t offset_fat = 0; byte_offset_fat(ctx, 0, &offset_fat);
    int32_t size_entry = 4; 
    char* buffer = malloc(size_entry);
    offset_fat += 4 * cluster; //32bit entries == 4 bytes
    ((PIO_DEVICE)(ctx->driver))->read(offset_fat, &size_entry, &buffer);
    if (buffer[0] == 0 && buffer[1] == 0 && buffer[2] == 0 && buffer[3] == 0) {
        return FAT_CLUST_FREE;
    } else if (buffer[0] == 0 && buffer[1] == 0 && buffer[2] == 0 && buffer[3] == 1) {
        return FAT_CLUST_RESERVED;
    } else if ((buffer[3] & 2) == 2) {
        *next = 99; //TODO - actually do this
        return FAT_CLUST_POINTER;
    } else if (buffer[3] == 0xF) {
        return FAT_CLUST_LAST;
    } else {
        return FAT_CLUST_BAD;
    }
}

//in case I don't care about new start byte
status_t traverse_and_find_file_rec(PFAT_CONTEXT ctx, int32_t start, char* path, FAT_DENTRY* out) {
    int32_t junk = start;
    return traverse_and_find_file_rec_r(ctx, &junk, path, out);
}

/** 
 * pre-condition: start is pointing at the head of a directory table
 * TODO - verify that start is updated to be the first byte of the file dentry (not name dentry)
 */
status_t traverse_and_find_file_rec_r(PFAT_CONTEXT ctx, int32_t* start, char* path, FAT_DENTRY* outv) {
    //base case: empty path (shouldn't happen, return err)
    //pull out first part of path
    char* pathhead = io_path_head(path); //note: need to free
    char* pathtail = io_path_head(path);
    //scan through directory, examining pathnames
    int32_t newstart = *start;
    int32_t current_clust = cluster_num_for_byte_offset(ctx, newstart);
    FAT_DENTRY file = parse_dirtable_entry(ctx, newstart, &newstart, current_clust);
    while (file.attr != NULL) {
        if (io_path_canonicalized_compare(pathhead, file.name) == 0) {
            if ((file.attr & 0x10) == FAT_DE_DIRECTORY && pathtail != NULL){ 
                return traverse_and_find_file_rec_r(ctx, &newstart, pathtail, outv);
            } else if ((file.attr & 0x10) != FAT_DE_DIRECTORY && pathtail == NULL) {
                *outv = file;
                return E_SUCCESS;
            }
            return E_NO_MATCH;
        }
        //correct byte offset for next iter if necessary:
        if (newstart - *start > ctx->cluster_size) {
            //find current cluster
            current_clust = cluster_num_for_byte_offset(ctx, newstart);
            //look up entry in fat
            int32_t next_clust = 0;
            FAT_ENTRY type = classify_fat_entry(ctx, current_clust, &next_clust);
            //if entry is end of block, return file not found
            //if entry is pointer, newstart = byteoffset for newblock
            if (type == FAT_CLUST_POINTER) {
                *start = next_clust;
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

status_t grab_next_free_clusters(PFAT_CONTEXT ctx, int32_t filesize, int32_t* start) {
    //TODO - update to support multi-cluster initial allocations
    //get fat offset
    int32_t starting_offset;
    int32_t size = 4;
    byte_offset_fat(ctx, 0, &starting_offset);
    starting_offset += 2*4; //skip first two entries due to special clusters
    char buffer[4];
    while (buffer[0] != 0 || buffer[1] != 0 || buffer[2] != 0 || buffer[3] != 0) {
        ((PIO_DEVICE)ctx->driver)->read(starting_offset, &size, &buffer);
        starting_offset += 4;
    }
    int32_t reserve = 0xFFFFFFFF;
    ((PIO_DEVICE)ctx->driver)->write(starting_offset-size, &size, &reserve);
}

status_t expand_cluster_in_fat(PFAT_CONTEXT ctx, int32_t start_cluster, int32_t newsize) {
    //first traverse through to end of cluster chain
    int32_t starting_offset;
    int32_t size = 4;
    byte_offset_fat(ctx, 0, &starting_offset);
    starting_offset += start_cluster*4;
    int32_t active_offset = starting_offset;
    char buffer[4];
    int32_t tail_cluster = 0;
    ((PIO_DEVICE)ctx->driver)->read(active_offset, &size, &buffer);
    while(((buffer[2] << 8 ) | buffer[3]) < 0xF8) {
        tail_cluster = active_offset;
        int32_t cluster = (buffer[20] << 24) | (buffer[21] << 16) | (buffer[26] << 8) | buffer[27];
        active_offset = starting_offset + 4 * cluster;
        ((PIO_DEVICE)ctx->driver)->read(active_offset, &size, &buffer);
    }
    //use grab_next_free_cluster to get one or more clusters
    int32_t newtail = 0;
    grab_next_free_clusters(ctx, newsize, &newtail);
    //replace end_chain entry with newly returned entry
    newtail = newtail << 1; //TODO - re-examine this math
    ((PIO_DEVICE)ctx->driver)->write(tail_cluster, &size, &newtail);
    return E_SUCCESS;
}
