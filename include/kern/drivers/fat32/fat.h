/**
 * functions and data structures related to working with the FAT directly. 
 */

#include "fat_types.h"
#pragma once
/**
 * \brief returns the byte offset on disk for a particular cluster
 * \param ctx [in] context block describing a particular FAT32 instance
 * \param cluster [in] cluster which we are finding offset for
 * \param out [out] byte offset from 0
 */
status_t byte_offset_for_cluster(PFAT_CONTEXT ctx, int32_t cluster, int32_t* out);

/**
 * \brief Finds the byte offset on disk of one of the two File Allocation Tables
 * \param ctx [in] context block describing a particular FAT32 instance
 * \param fat_select [in] selects the FAT table to return; 0 is primary, 1 is backup
 * \param out [out] byte offset on disk
 */
status_t byte_offset_fat(PFAT_CONTEXT ctx, int32_t fat_select, int32_t* out);

/**
 * brief Finds a file on disk by name
 * \param ctx [in] context block describing a particular FAT32 instance
 * \param path [in] path of the file or directory to find. Note: path starts within the FAT32 filesystem, excluding the mount/volume/drive component
 * \param out [out] D-Entry object describing a file
 */
status_t find_file(PFAT_CONTEXT ctx, char* path, FAT_DENTRY* out);

/**
 * \brief gets the next entry in a directory
 * NOTE: this assumes that the parent will point to a valid D-Entry *pointing to* a directory
 * \param ctx [in] context block describing a particular instance of FAT32
 * \param parent [in] pointer to directory we're reading from
 * \param offset [in] offset into the directory table
 * \param dentry [out]
 */
status_t read_next_dentry(PFAT_CONTEXT ctx, FAT_DENTRY* parent, int32_t offset, FAT_DENTRY* dentry);

