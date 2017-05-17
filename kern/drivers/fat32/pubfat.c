/**
 * \file pubfat.c
 * \author Nathan C. Castle
 * \brief public interface to FAT32 filesystem
 */

#include <kern/drivers/fat32/pubfat.h>

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
    FAT_CONTEXT ctxt = match_context(msg);
    switch (msg->ioctl) {
        case IOCTL_READ:
            //recursively find file
            //seek into file
            //read in
            //update last accessed date
            break;
        case IOCTL_APPEND:
            //recursively find file
            //seek into file
            //expand file size
            //update last modified
            //create new clusters if necessary
            //update FAT if necessary
            //begin write
            break;
        case IOCTL_WRITE:
            //recursively find file
            //seek into file
            //update last modified
            //expand file size if necessary
            //create new clusters if necessary
            //update FAT if necessary
            //begin write
            break;
        case IOCTL_GROW:
        case IOCTL_EXPAND:
            //recursively find file
            //seek into file
            //update last modified
            //expand file size if necessary
            //update FAT if necessary
            //zero out data in file
            break;
        case IOCTL_DELETE:
            //recursively find file
            //clear out name entries
            //clear out base entry
            //update FAT if necessary
            //return
            break;
        case IOCTL_QUERY:
            //recursively find file
            //update last accessed property (if appropriate)
            //read/calc relevant property
            break;
        case IOCTL_CREATE:
            //recursively find parent
            //calculate long name entries
            //write new long name and directory entries
            //update FAT table if necessary
            break;
        case IOCTL_ENUMERATE:
            //recursively find parent directory
            //figure out which the current file is
            //fiure out which the next file is
            //return next file
            break;
        case IOCTL_SHRINK:
            //recursively find file
            //adjust the dentry size
            //adjust clusters/FAT if necessary
            break;
        case IOCTL_IDENTIFY:
            //recursively find file
            //return if there
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
