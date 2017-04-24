/**
 * \author Nathan C. Castle
 * \file file.c
 * \brief Implements the File-Oriented I/O API.
 *        This is intended for consumption by user-mode apps
 */

#include "file.h"
status_t IoFileOpen (char* path, IOCREATEPOLICY strat, PFILEHANDLE out){
    //ensure data structures initialized

    //allocate table entry

    //update table entry
    
    //Look for File, create if required by policy

    return E_NOT_IMPLEMENTED;
}

status_t IoFileRead (FILEHANDLE file, BSIZE offset, PBSIZE plength, void* out){
    //verify handle

    //form IO request

    //send IO request
    
    //verify status & return
    return E_NOT_IMPLEMENTED;
}
status_t IoFileWrite (FILEHANDLE file, BSIZE offset, PBSIZE plength, void* in){
    //verify handle

    //form IO request

    //send IO request

    //verify status & return
    return E_NOT_IMPLEMENTED;
}
status_t IoFileSeek (FILEHANDLE file, BSIZE offset, PBSIZE poffset){
    //verify handle

    //update cursor

    //return status
    return E_NOT_IMPLEMENTED;
}
status_t IoFileQuery (FILEHANDLE file, IOPROP property, PBSIZE plength, void* out){
    //verify handle
    
    //verify metadata; fetch if needed

    //form output & return
    return E_NOT_IMPLEMENTED;
}
status_t IoFileSet (FILEHANDLE file, IOPROP property, PBSIZE plength, void* in){
    //verify handle

    //update metadata block

    //flush changes
    return E_NOT_IMPLEMENTED;
}
status_t IoFileDelete (FILEHANDLE file){
    //verify handle

    //build IO Message & send

    //close file handle if delete succesful
    //return status
    return E_NOT_IMPLEMENTED;
}
status_t IoFileNextChild (FILEHANDLE file, IOPROP property, IOSORT sort, int index, PBSIZE plength, char* out){
    //verify handle

    //fetch metadata if necessary

    //verify index & return data
    return E_NOT_IMPLEMENTED;
}
status_t IoFileClose (FILEHANDLE file){
    //verify handle
    
    //close the handle
    return E_NOT_IMPLEMENTED;
}
