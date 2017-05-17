/**
 * \file fat_types.h
 * \brief types and structures used for representing fat32 info
 * \author Nathan C. Castle
 */

#include <baseline/common.h>
#pragma once

typedef enum _fat_clust_stat {
    FAT_CLUST_FREE,
    FAT_CLUST_RESERVED,
    FAT_CLUST_POINTER, //used; points to next clust
    FAT_CLUST_BAD,
    FAT_CLUST_LAST
} FAT_ENTRY;

//takes a cluster entry and determines its type
FAT_ENTRY classify_fat_entry(int32_t fat_entry_value); 

typedef struct _ft_fsinfo {
    char compatguard[4]; //default 0x52 0x52 0x61 0x41
    char reserved[480]; //default: 0
    char info_sect_sig[4]; //default: 0x72 0x72 0x41 0x61
    char cnt_free_clust[4]; //last known number of free clusters; unreliable; default 0xFFFFFFFF
    char num_most_recent_clust[4]; //number of most recent allocated; default 0xFFFFFFFF
    char reserved2[12]; //default: 0
    char sector_sig[4]; //default: 0x00 0x00 0x55 0xAA
} FS_INFO_SECTOR;

typedef struct _ft_bpb { //2x 512byte sectors 
    char bootguard[3]; //EB 3C 90
    char compat1[8]; //MSWIN4.1
    char sectorbytes[2]; //needs to be a number; default 512
    char sectorspercluster[1]; //default 64; 32Kb clusters
    char cntreservedsectors[2]; //default: 2?
    char cntfats[1]; //# of fats; default 2
    char cntfat1216[2]; //# of FAT12/FAT16 RDEs; 0
    char cntsectors[2]; //0 if greater than 65535
    char medescriptype[1]; //media descriptor type; default F8
    char cntsectperfat[2]; //FAT12/16 compat; default 0; ignored
    char cntsectpertrack[2]; //0
    char cntheadsdisk[2]; //0
    char cntpart[4]; //0 - no partitioning support
    char cntlrgsectors[4]; //used if sector count > 65535
    //FAT32 Extended Fields
    char cntclustfat[4]; //clusters per fat
    char flags[2]; //default 0; ignored
    char fatversion[2]; //default: ???
    char clustnumroot[4]; //cluster # root dir; defaults to 2
    char sectnmfsinfo[2]; //sector # of FS Info structure; default: 1
    char numbackupsect[2]; //sector # backup boot sect; default 0
    char reserved[12]; //default 0
    char numdriver[1]; //default 80; ignore otherwise
    char ntflags[1]; //default 0; ignored
    char signature[1]; //default: 0x28
    char volid[4]; //default 0; generally ignored
    char vol_label_string[11]; //padded with space; default: AlphaGoS
    char sys_ident_str[8]; //default: FAT32   ; otherwise ignored
    char bootcode[420]; //default empty
    char bootablepartsig[2]; //default: 0xAA55
    FS_INFO_SECTOR fsinfo; //second 512 byte sector
} BIOS_PARAM_BLOCK;

typedef enum _ft_de_attr { //one byte
    FAT_DE_READONLY = 0x01, //long file names only
    FAT_DE_HIDDEN = 0x02, //long file names only
    FAT_DE_SYSTEM = 0x04, //long file names only
    FAT_DE_VOLUMEID = 0x08, //Long file names only
    FAT_DE_DIRECTORY = 0x10,
    FAT_DE_ARCHIVE = 0x20
} FAT_DE_ATTRIBUTE;

typedef struct _f32_time {
    int8_t  hour : 5;
    int8_t  minute : 6;
    int8_t  seconds : 5;
    int8_t  year : 7;
    int8_t  month: 4;
    int8_t  day: 5;
} FAT32TIME, *PFAT32TIME;

typedef struct _ft_time_packed { //two bytes
    unsigned int hour : 5;
    unsigned int minutes : 6;
    unsigned int seconds : 5; //mult by 2
} FAT_TIME_PACKED;

typedef struct _ft_date_packed { //two bytes
    unsigned int year : 7;
    unsigned int month : 4;
    unsigned int day : 5;
} FAT_DATE_PACKED;

typedef struct _ft_de_sd { //32 bytes
    char entry_stat[1]; //0 if no more entries; 0xE5 if unused; 0x1 if used
    char entry_stat_reserved[10]; //default 0; compat
    char attribute[1]; //see FAT_DE_ATTRIBUTE
    char reservednt[1]; //reserved for Windows NT
    char create_time_tenths_s[1];  //creation time in tenths of second, 0-199 inclusive
    FAT_TIME_PACKED create_time;
    FAT_DATE_PACKED create_date;
    FAT_DATE_PACKED last_accessed;
    char highbits_cluster[2]; //high 16 bits of cluster #
    FAT_TIME_PACKED last_mod_time;
    FAT_DATE_PACKED last_mod_date;
    char lowbits_cluster[2]; //low 16 bits of cluster #
    char filesize[4]; //size of file in bytes
} FAT_DE_ENTRY;

typedef struct _ft_de_longname {
    char index[1]; //order relative to other name components
    char firstchars[10]; //first 5 2-byte chars (think: unicode)
    char constant[1]; //always 0x0F; identifies this as name block
    char constanttwo[1]; //always 0
    char legacychecksum[1]; //checksum of 8.3; ignored for patent reasons
    char nextchars[12]; //next 6 2-byte chars
    char constantthree[2]; //always 0
    char finalchars[4]; //last 2 2-byte chars in name
} FAT_DE_LONGNAME_ENTRY;

//in-memory structure for representing directory entry
typedef struct _ft_de_inmem {
    char* name; //single-byte characters
    int32_t created;
    int32_t read;
    int32_t updated;
    int32_t filesize;
    int32_t cluster_start;
    int32_t byte_offset_start;
    int32_t cluster_parent_de;
    int32_t child_count;
    FAT_DE_ATTRIBUTE attr;
} FAT_DENTRY;

typedef int* FAT_CACHE;

typedef struct _ft_ctxt_block {
    FAT_CACHE   cache;
    void*       filesystem;
    void*       mount;
    void*       driver;
    int32_t     sector_size;
    int32_t     cluster_size;
    int32_t     clusters_per_fat;
} FAT_CONTEXT, *PFAT_CONTEXT;
