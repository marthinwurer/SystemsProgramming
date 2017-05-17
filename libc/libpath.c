/**
 * \author Nathan C. Castle
 * \file libpath.c
 * \brief Implements run-time IO path manipulation support
 */

#include "libpath.h"
int32_t  io_path_count_nodes(char* path) {
    int compareIndex = 0;
    int pathcount = 0;
    char lastChar = '\0';
    char currentChar = path[compareIndex];
    while (currentChar != '\0'){
        if (currentChar == IO_PATH_DELIMITER){
            if (compareIndex != 0 && currentChar != lastChar){
                pathcount++;
            }
        }
        compareIndex++;
        lastChar = currentChar;
        currentChar = path[compareIndex];
    }
    if (lastChar != IO_PATH_DELIMITER && lastChar != '\0'){
        pathcount++;
    }
    return pathcount;
}

void io_path_keep_n_nodes(char* path, unsigned int n, char* out) {
    int compareIndex = 0;
    unsigned int pathcount = 0;
    char lastChar = '\0';
    char currentChar = path[compareIndex];
    while (currentChar != '\0'){
        out[compareIndex] = currentChar;
        if (currentChar == IO_PATH_DELIMITER){
            if (compareIndex != 0 && currentChar != lastChar){
                pathcount++;
            }
        }
        if (pathcount == n){
            path[compareIndex++] = '\0';
            return;
        }
        compareIndex++;
        lastChar = currentChar;
        currentChar = path[compareIndex];
    }
}

char* io_path_tail(char* path) {
    return NULL; //TODO implement
}

char* io_path_head(char* path) {
    return NULL; //TODO implement
}

void io_path_disc_n_nodes(char* path, unsigned int n, char* out) {
    int remaining = io_path_count_nodes(path) - n;
    io_path_keep_n_nodes(path, remaining, out);
}

int32_t io_path_canonicalized_compare(char* path1, char* path2) {
    int lIndex = 0;
    int rIndex = 0;
    int result = 0;
    int lTokens = 0;
    int rTokens = 0;

    do {
        if (path1[lIndex] == IO_PATH_DELIMITER){
            lIndex++;
        }
        if (path2[rIndex] == IO_PATH_DELIMITER){
            rIndex++;
        }
    } while (path1[lIndex] == IO_PATH_DELIMITER || path2[rIndex] == IO_PATH_DELIMITER);
    while (path1[lIndex] != '\0' && path2[rIndex] != '\0'){
        lIndex++; rIndex++;
        do {
            if (path1[lIndex] == '\0' || path2[rIndex] == '\0'){
                break;
            }
            if (path1[lIndex] == IO_PATH_DELIMITER && path1[lIndex -1] != IO_PATH_DELIMITER){
                lTokens++;
                lIndex++;
            }
            if (path2[rIndex] == IO_PATH_DELIMITER && path2[rIndex - 1] != IO_PATH_DELIMITER) {
                rTokens++;
                rIndex++;
            } 

        } while ( lTokens != rTokens);
        if (path1[lIndex] != path2[rIndex]){
            return 1; //mismatch
        }
    }
    if (path1[lIndex] != path2[rIndex]) { //one side was empty
        return 1;
    }
    return result;
}
