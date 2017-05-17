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

/**
 * \brief grabs a new cluster and returns its number. If the filesize is larger than
 *        can be held in a single cluster, a cluster chain is set up
 * \param ctx [in] context block describing a particular instance of FAT32
 * \param filesize [in] size of the file run needed in bytes
 * \param start [out] return parameter, cluster # of the first of the cluster
 */
status_t grab_next_free_clusters(PFAT_CONTEXT ctx, int32_t filesize, int32_t* start);

/**
 * \brief takes an existing cluster and expands or contracts to fit the new file size
 * \param ctx [in] context block describing a particular instance of FAT32
 * \param start_cluster [in] # of the starting cluster
 * \param newsize [in] new size of the file in bytes
 */
status_t expand_cluster_in_fat(PFAT_CONTEXT ctx, int32_t start_cluster, int32_t newsize);

/**
 * \brief frees a cluster chain, starting from the start cluster through to the end
 * \param ctx [in] context block describing a particular instance of FAT32
 * \param cluster_start [in] # of the cluster to remove (or start cluster in chain)
 */
status_t free_cluster_chain(PFAT_CONTEXT ctx, int32_t cluster_start);

/**
 * \brief updates a directory entry for a file with provided values
 * \param ctx [in] context block describing an instance of the FAT32 filesystem
 * \param parent_dir_cluster [in] cluster # of the directory containing relevant dentry
 * \param updated [in] D-ENTRY object containing information to be updated
 */
status_t update_dentry(PFAT_CONTEXT ctx, int32_t parent_dir_cluster, FAT_DENTRY updated);

/**
 * \brief wipe out all D-entries associated with a file
 * \param ctx [in] context block describing instance of FAT32
 * \param parent_dir_cluster [in] # of cluster containing directory with dentry to delete
 * \param name [in] name of file for which D-entries will be deleted
 */
status_t wipe_dentry(PFAT_CONTEXT ctx, int32_t parent_dir_cluster, char* name);

/**
 * \brief writes a buffer into a file at specified location
 * NOTE: DOES NOT RESIZE FILE; call expand_cluster_in_fat ahead of time to resize
 * \param ctx [in] context block describing an instance of the FAT32 filesystem
 * \param cluster_start [in] starting cluster of the file to write to
 * \param write_size [in] number of bytes to write
 * \param offset [in] position in file to write to
 * \param buffer [in] data to write to file
 */
status_t write_into_cluster_chain(PFAT_CONTEXT ctx, int32_t cluster_start, int32_t write_size, int32_t offset, char* buffer);
