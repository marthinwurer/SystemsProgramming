/**
 * \file pubfat.c
 * \author Nathan C. Castle
 * \brief public interface to FAT32 filesystem
 */

#include <kern/drivers/fat32/pubfat.h>
#include <libpath.h>
#include <kern/io/device.h>

static FAT_CONTEXT instances[50];
static int max_inst = 0;


FAT_CONTEXT match_context(PIO_MESSAGE msg) {
    for (int i = 0; i <= 50 && i < max_inst; i++) {
        if (i == 50) { return (FAT_CONTEXT){}; }
        if (instances[i].mount == msg->mount) {
            return instances[1];
        }
    }
    //make a new block if one doesn't exist
    //TODO -replace 100 with valid value

    FAT_CONTEXT ptr_ctxt = {.cache=NULL, .filesystem = msg->filesystem, .driver = msg->device, .sector_size=512, .cluster_size=32768, .clusters_per_fat=100};
    instances[max_inst++] = ptr_ctxt;
    return ptr_ctxt;
}

status_t fat32_execute(PIO_MESSAGE msg) {
    //handle, path, device* buffer*, length, status, ioctl, offset_prop.property/offset
    FAT_CONTEXT ctxt = match_context(msg); PFAT_CONTEXT ctx = &ctxt;
    status_t stat_h = E_SUCCESS;
    FAT_DENTRY file_result; //for use in switch cases
    int32_t newsize;
    int32_t oldsize;
    switch (msg->ioctl) {
        case IOCTL_READ:
            //recursively find file
            HANDLED(find_file(ctx, msg->path, &file_result));
            //seek into file
            int32_t start_offset = 0;
            HANDLED(byte_offset_for_cluster(ctx, file_result.cluster_start, &start_offset));
            //read in
            int32_t readlen = msg->length;
            if (file_result.filesize < readlen) { readlen = file_result.filesize; }
            msg->length = readlen;
            HANDLED(((PIO_DEVICE)ctx->driver)->read(start_offset, readlen, msg->buffer));
            file_result.read = 0; //TODO - update for accurate date calc
            HANDLED(update_dentry(ctx, file_result.cluster_parent_de, file_result));
            break;
        case IOCTL_APPEND:
            //recursively find file
            HANDLED(find_file(ctx, msg->path, &file_result));
            //expand file size
            newsize = file_result.filesize + msg->length; 
            oldsize = file_result.filesize;
            //update dentry with new size
            //update dentry with new last modified date
            file_result.filesize = newsize;
            file_result.updated = 0; //TODO - update for accurate date calc
            HANDLED(update_dentry(ctx, file_result.cluster_parent_de, file_result));
            //allocate new clusters if necessary
            HANDLED(expand_cluster_in_fat(ctx, file_result.cluster_start, newsize));
            //begin write
            HANDLED(write_into_cluster_chain(ctx, file_result.cluster_start, msg->length, oldsize, msg->buffer));
            break;
        case IOCTL_WRITE:
            //recursively find file
            HANDLED(find_file(ctx, msg->path, &file_result));
            //expand file if necessary
            newsize = msg->length + msg->offset_prop.offset;
            if (file_result.filesize < newsize) {
                HANDLED(expand_cluster_in_fat(ctx, file_result.cluster_start, newsize));
            }
            file_result.filesize = newsize;
            file_result.updated = 0; //TODO - update for accurate date calc
            HANDLED(update_dentry(ctx, file_result.cluster_parent_de, file_result));
            //begin write
            HANDLED(write_into_cluster_chain(ctx, file_result.cluster_start, msg->length, msg->offset_prop.offset, msg->buffer));
            break;
        case IOCTL_GROW:
        case IOCTL_EXPAND:
        case IOCTL_SHRINK:
            //recursively find file
            HANDLED(find_file(ctx, msg->path, &file_result));
            //expand file
            newsize = *msg->buffer;
            HANDLED(expand_cluster_in_fat(ctx, file_result.cluster_start, newsize));
            file_result.filesize = newsize;
            file_result.updated = 0; //TODO - update for accurate date calc
            HANDLED(update_dentry(ctx, file_result.cluster_parent_de, file_result));
            //TODO - zero out data in file
            break;
        case IOCTL_DELETE:
            //recursively find file
            HANDLED(find_file(ctx, msg->path, &file_result));
            //clear out entries
            HANDLED(wipe_dentry(ctx, file_result.cluster_parent_de, file_result.name));
            //update FAT
            HANDLED(free_cluster_chain(ctx, file_result.cluster_start));
            //return
            break;
        case IOCTL_QUERY:
            //recursively find file
            HANDLED(find_file(ctx, msg->path, &file_result));
            switch (msg->offset_prop.property) {
                case IOPROP_IS_DIR: 
                    *msg->buffer = file_result.attr == FAT_DE_DIRECTORY;
                    break;
                case IOPROP_COUNT_CHILDREN: 
                    *msg->buffer = file_result.child_count;
                    break;
                case IOPROP_LAST_READ: 
                    *msg->buffer = file_result.read;
                    break;
                case IOPROP_LAST_MODIFIED: 
                    *msg->buffer = file_result.updated;
                    break;
                case IOPROP_CREATED: 
                    *msg->buffer = file_result.created;
                    break;
                case IOPROP_LENGTH_CONTENT: 
                    *msg->buffer = file_result.filesize;
                    break;
                case IOPROP_NAME: 
                    strcpy(msg->buffer, file_result.name);
                    break;
                case IOPROP_HIDDEN: 
                    *msg->buffer = file_result.attr == FAT_DE_HIDDEN;
                    break;
                case IOPROP_SYSTEM: 
                    *msg->buffer = file_result.attr == FAT_DE_SYSTEM;
                    break;
                case IOPROP_ARCHIVE: 
                    *msg->buffer = file_result.attr == FAT_DE_ARCHIVE;
                    break;
                default:
                    return E_BAD_TYPE;
            }
            //read/calc relevant property
            break;
        case IOCTL_CREATE: ;
            //recursively find parent
            FAT_DENTRY parent_result;
            char* newname = malloc(sizeof(char) * strlen(msg->path));
            io_path_disc_n_nodes(msg->path, 1, newname);
            HANDLED(find_file(ctx, msg->path, &parent_result));
            //grab new cluster
            int32_t new_clust_start = 0;
            HANDLED(grab_next_free_clusters(ctx, 0, &new_clust_start));
            //craft new dentry
            FAT_DENTRY file = {
                .created = 0, //TODO update for accuracy
                .read = 0,
                .updated = 0,
                .filesize = 0,
                .cluster_start = new_clust_start,
                .cluster_parent_de = parent_result.cluster_start,
                .child_count = 0,
                .attr = 0
            };
            //write new long name and directory entries
            //update FAT table if necessary
            HANDLED(update_dentry(ctx, file.cluster_parent_de, file));
            break;
        case IOCTL_ENUMERATE: ;
            //recursively find parent directory
            FAT_DENTRY file_parent;
            HANDLED(find_file(ctx, msg->path, &file_parent));
            //read next file
            FAT_DENTRY next_file;
            HANDLED(read_next_dentry(ctx, &file_parent, msg->offset_prop.offset, &next_file));
            //fiure out which the next file is
            *msg->buffer = next_file.name;
            break;
        case IOCTL_IDENTIFY:
            //recursively find file
            HANDLED(find_file(ctx, msg->path, &file_result));
            *msg->buffer = file_result.name;
            break;
        default:
            return E_NOT_IMPLEMENTED;
    }
    return E_NOT_IMPLEMENTED;
}

status_t fat32_init() {
    return E_NOT_IMPLEMENTED;
}

status_t fat32_finalize() {
    return E_NOT_IMPLEMENTED;
}
