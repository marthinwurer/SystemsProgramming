/**
 * \author Nathan C. Castle
 * \file file.c
 * \brief Implements the File-Oriented I/O API.
 *        This is intended for consumption by user-mode apps
 */

#include <kern/ioapi/file.h>
#include <kern/io/router.h>
#include <libpath.h>

typedef struct _io_file_instance {
    FILEHANDLE          handle;         //ID of the file
    IOHANDLE            message_in;        //ID of the I/O Message object
    IOHANDLE            message_out;
    char*               path;
    BSIZE               length;
    pid_t               owner;
    BSIZE               cursor;
    IO_UNIX_TS          created;
    IO_UNIX_TS          written;
    IO_UNIX_TS          read;
} io_file_entry, *pio_file_entry;

#define FILES_MAX 50
static io_file_entry FILETABLE[FILES_MAX]; //statically allocate file object
static int _iofs_has_init = 0;
/**
 * \brief Initialize filesystem API data structures
 */
void _iofs_init() {
    for (int index = 0; index <FILES_MAX; index++){
        FILETABLE[index] = (io_file_entry) { .handle = -1,
            .message_in = IOHANDLE_NULL,
            .message_out = IOHANDLE_NULL,
            .path = NULL,
            .length = 0,
            .owner = 0,
            .cursor = 0,
            .created = 0,
            .written = 0,
            .read = 0
        };
    }
    _iofs_has_init = 1;
}

int32_t _iofs_handle_is_valid(FILEHANDLE handle){
    if (handle < 0){
        return 0;
    }
    if (handle >= FILES_MAX){
        return 0;
    }
    if (FILETABLE[handle].handle != handle){
        return 0;
    }
    return 1;
}
status_t IoFileOpen (char* path, IOCREATEPOLICY strat, PFILEHANDLE out){
    //ensure data structures initialized
    if (_iofs_has_init == 0){
        _iofs_init();
    }
    int index;
    //allocate table entry
    for (index = 0; index < FILES_MAX; index++){
        if (FILETABLE[index].handle == -1){
            FILETABLE[index].handle = index;
            break;
        }
    }
    pio_file_entry fileatindex = &FILETABLE[index];
    //look for file
    status_t stat = E_SUCCESS;
    ENABLEHANDLERS
    pretty_print(IO_PROTOTYPE(IO_OBJ_MESSAGE, &(fileatindex->message_in)));
    IOHANDLE hMessage = fileatindex->message_in;
    HANDLED(IO_UPDATE_STR(hMessage, IOPROP_PATH, path));
    HANDLED(IO_UPDATE_IOCTL(hMessage, IOCTL_IDENTIFY));
    stat = IO_EXECUTE(hMessage);
    pretty_print(stat);
    if (stat == E_SUCCESS){
        *out = index;
        return E_SUCCESS;
    } 
    char newpath[strlen(path)]; //because we can't use variable allocation inside switch
    int mountindex = 0;
    //file not found, handle that
    switch (strat) {
        case IO_CP_CREATE: 
            //strip last element from path
            io_path_disc_n_nodes(path, 1, newpath);
            //check for existence of new path
            HANDLED(IO_UPDATE_STR(hMessage, IOPROP_PATH, newpath));
            stat = IO_EXECUTE(hMessage);
            //if exists, create original path
            if (stat != E_SUCCESS) { fileatindex->handle = -1; return E_NO_MATCH; }
            IGNORED(IO_UPDATE_STR(hMessage, IOPROP_PATH, path));
            IGNORED(IO_UPDATE_IOCTL(hMessage, IOCTL_CREATE));
            stat = IO_EXECUTE(hMessage);
            if (stat == E_SUCCESS) {
                *out = index;
            } else {
                fileatindex->handle = -1;
                return stat;
            }
        case IO_CP_CREATERECURSIVE: 
            mountindex = 0;
            //verify mount is valid
            IOHANDLE mount = IOHANDLE_NULL;
            while (stat == E_SUCCESS) {
                stat = IO_ENUMERATE(IO_OBJ_MOUNT, mountindex, &mount);
                int length = 0; //following E_MORE_DATA pattern
                IGNORED(IO_INTERROGATE(mount, IOPROP_PATH, (void*)NULL, &length));
                char outpath[length];
                HANDLED(IO_INTERROGATE(mount, IOPROP_PATH, outpath, &length));
                if (io_path_canonicalized_compare(path, outpath)){ mountindex = -1; break; } 
                mountindex++;
            }
            if (mountindex < 0){
                //iterate through path components, making as necessary
                int numpathparts = io_path_count_nodes(path);
                for (int pathind = 1; pathind < numpathparts; pathind++){
                    //get path component
                    int length = strlen(path);
                    char outpath[length];
                    io_path_keep_n_nodes(path, pathind, outpath);
                    HANDLED(IO_UPDATE_STR(hMessage, IOPROP_PATH, outpath));
                    HANDLED(IO_UPDATE_IOCTL(hMessage, IOCTL_CREATE));
                    stat = IO_EXECUTE(hMessage);
                    if (stat != E_SUCCESS){
                        fileatindex->handle = -1;
                        return E_NO_MATCH;
                    }
                }
            } else {
                IO_DELETE(hMessage);
                return E_NO_MATCH;
            }
            //finished, return success
            *out = index;
            return E_SUCCESS;
            break;
        case IO_CP_FAIL:
        default:
            fileatindex->handle = -1;
            return E_NO_MATCH;
    }
    return E_SUCCESS;
}

status_t IoFileRead (FILEHANDLE hFile, BSIZE offset, PBSIZE plength, void* out){
    //verify handle
    if (!_iofs_handle_is_valid(hFile)) { return E_BAD_HANDLE; }
    pio_file_entry file = &FILETABLE[hFile];
    //open or create IO Message
    ENABLEHANDLERS //used by HANDLED
    if (file->message_in == IOHANDLE_NULL){
        //update message
        HANDLED(IO_PROTOTYPE(IO_OBJ_MESSAGE, &file->message_in));
        HANDLED(IO_UPDATE_STR(file->message_in, IOPROP_PATH, file->path));
    }
    
    //update cursor
    file->cursor += offset;
    HANDLED(IO_UPDATE_VALINT(file->message_in, IOPROP_CURSOR_POSITION, file->cursor));
    //update message
    HANDLED(IO_UPDATE_VOID(file->message_in, IOPROP_BUFFER, out));
    HANDLED(IO_UPDATE_VALINT(file->message_in, IOPROP_LENGTH_CONTENT, *plength));
    HANDLED(IO_UPDATE_IOCTL(file->message_in, IOCTL_READ));
    //send IO request
    HANDLED(IO_EXECUTE(file->message_in));
    //verify status & return
    file->cursor += *plength;
    IGNORED(IO_UPDATE_VALINT(file->message_out, IOPROP_CURSOR_POSITION, file->cursor));
    IGNORED(IO_UPDATE_VALINT(file->message_in, IOPROP_CURSOR_POSITION, file->cursor));    //update cursor
    return E_SUCCESS;
}

status_t IoFileWrite (FILEHANDLE hFile, BSIZE offset, PBSIZE plength, void* in){
    //verify handle
    if (!_iofs_handle_is_valid(hFile)) { return E_BAD_HANDLE; }
    //form IO request
    pio_file_entry file = &FILETABLE[hFile];
    ENABLEHANDLERS //used by handled macros
    if (file->message_out == IOHANDLE_NULL) {
        HANDLED(IO_PROTOTYPE(IO_OBJ_MESSAGE, &file->message_out));
        HANDLED(IO_UPDATE_STR(file->message_out, IOPROP_PATH, file->path)); 
    }
    //update cursor
    file->cursor += offset;
    //update IO Request
    HANDLED(IO_UPDATE_VOID(file->message_out, IOPROP_BUFFER, in));
    HANDLED(IO_UPDATE_VALINT(file->message_out, IOPROP_LENGTH_CONTENT, *plength));
    HANDLED(IO_UPDATE_IOCTL(file->message_out, IOCTL_WRITE));
    //send IO request
    HANDLED(IO_EXECUTE(file->message_out));
    //verify status & return
    file->cursor += *plength;
    IGNORED(IO_UPDATE_VALINT(file->message_in, IOPROP_CURSOR_POSITION, file->cursor));
    IGNORED(IO_UPDATE_VALINT(file->message_out, IOPROP_CURSOR_POSITION, file->cursor));
    return E_SUCCESS;
}
status_t IoFileSeek (FILEHANDLE hFile, BSIZE offset, PBSIZE poffset){
    //verify handle
    if (!_iofs_handle_is_valid(hFile)) { return E_BAD_HANDLE; }
    pio_file_entry file = &FILETABLE[hFile];
    file->cursor = offset;
    ENABLEHANDLERS //used by HANDLED macros
    //update cursor
    if (file->message_in != IOHANDLE_NULL) { 
        HANDLED(IO_UPDATE_VALINT(file->message_in, IOPROP_CURSOR_POSITION, file->cursor));
    }
    if (file->message_out != IOHANDLE_NULL) { 
        HANDLED(IO_UPDATE_VALINT(file->message_out, IOPROP_CURSOR_POSITION, file->cursor));
    }
    *poffset = file->cursor;
    //return status
    return E_SUCCESS;
}
status_t IoFileQuery (FILEHANDLE hFile, IOPROP property, PBSIZE plength, void* out){
    //verify handle
    if (!_iofs_handle_is_valid(hFile)) { return E_BAD_HANDLE; }
    pio_file_entry file = &FILETABLE[hFile];
    //open or create IO Message
    ENABLEHANDLERS //used by HANDLED
    if (file->message_in == IOHANDLE_NULL){
        //update message
        HANDLED(IO_PROTOTYPE(IO_OBJ_MESSAGE, &file->message_in));
        HANDLED(IO_UPDATE_STR(file->message_in, IOPROP_PATH, file->path));
    }
    HANDLED(IO_UPDATE_VALINT(file->message_in, IOPROP_PROP, (int32_t)property));
    //update message
    HANDLED(IO_UPDATE_VOID(file->message_in, IOPROP_BUFFER, out));
    HANDLED(IO_UPDATE_VALINT(file->message_in, IOPROP_LENGTH_CONTENT, *plength));
    HANDLED(IO_UPDATE_IOCTL(file->message_in, IOCTL_QUERY));
    //send IO request
    RETURNED(IO_EXECUTE(file->message_in));
    //form output & return
}
status_t IoFileSet (FILEHANDLE hFile, IOPROP property, PBSIZE plength, void* in){
    //verify handle
    if (!_iofs_handle_is_valid(hFile)) { return E_BAD_HANDLE; }
    //update metadata block
    pio_file_entry file = &FILETABLE[hFile];
    ENABLEHANDLERS //used by handled macros
    if (file->message_out == IOHANDLE_NULL) {
        HANDLED(IO_PROTOTYPE(IO_OBJ_MESSAGE, &file->message_out));
        HANDLED(IO_UPDATE_STR(file->message_out, IOPROP_PATH, file->path));
    }
    HANDLED(IO_UPDATE_VALINT(file->message_out, IOPROP_PROP, (int32_t)property));
    //update IO Request
    HANDLED(IO_UPDATE_VOID(file->message_out, IOPROP_BUFFER, in));
    HANDLED(IO_UPDATE_VALINT(file->message_out, IOPROP_LENGTH_CONTENT, *plength));
    HANDLED(IO_UPDATE_IOCTL(file->message_out, IOCTL_WRITE));
    //send IO request
    RETURNED(IO_EXECUTE(file->message_out));
}
status_t IoFileDelete (FILEHANDLE hFile){
    //verify handle
    if (!_iofs_handle_is_valid(hFile)) { return E_BAD_HANDLE; }
    ENABLEHANDLERS
    //build IO Message & send
    pio_file_entry file = &FILETABLE[hFile];
    if (file->message_out == IOHANDLE_NULL) {
        HANDLED(IO_PROTOTYPE(IO_OBJ_MESSAGE, &file->message_out));
        HANDLED(IO_UPDATE_STR(file->message_out, IOPROP_PATH, file->path));
    };
    HANDLED(IO_UPDATE_IOCTL(file->message_out, IOCTL_DELETE));
    HANDLED(IO_EXECUTE(file->message_out));
    IGNORED(IO_DELETE(file->message_in));
    IGNORED(IO_DELETE(file->message_out));
    //close file handle if delete succesful
    file->message_in = IOHANDLE_NULL;
    file->message_out = IOHANDLE_NULL;
    file->handle = IOHANDLE_NULL;
    //return status
    return E_SUCCESS;
}
status_t IoFileNextChild (FILEHANDLE hFile, int index, PBSIZE plength, char* out){
    //verify handle
    if (!_iofs_handle_is_valid(hFile)) { return E_BAD_HANDLE; }
    pio_file_entry file = &FILETABLE[hFile];
    ENABLEHANDLERS
    //open input handle
    if (file->message_in == IOHANDLE_NULL) {
        HANDLED(IO_PROTOTYPE(IO_OBJ_MESSAGE, &file->message_in));
        HANDLED(IO_UPDATE_STR(file->message_in, IOPROP_PATH, file->path));
    }
    //update input handle
    HANDLED(IO_UPDATE_VOID(file->message_in, IOPROP_BUFFER, out));
    HANDLED(IO_UPDATE_VALINT(file->message_in, IOPROP_LENGTH_CONTENT, *plength));
    HANDLED(IO_UPDATE_IOCTL(file->message_in, IOCTL_ENUMERATE));
    HANDLED(IO_UPDATE_VALINT(file->message_in, IOPROP_CURSOR_POSITION, index));
    //execute & return
    RETURNED(IO_EXECUTE(file->message_in));
}
status_t IoFileClose (FILEHANDLE hFile){
    //verify handle
    if (!_iofs_handle_is_valid(hFile)) { return E_BAD_HANDLE; }
    pio_file_entry file = &FILETABLE[hFile];
    IGNORED(IO_DELETE(file->message_out));
    IGNORED(IO_DELETE(file->message_in));
    file->handle = -1;
    //close the handle
    return E_SUCCESS;
}
