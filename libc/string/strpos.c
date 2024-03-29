/*
** File: libc/string/strpos.c
**
** Author: Nathan Castle (nxc4524)
**
** Contributors:
**
** Contains the strpos function implementation
**
*/

#include <string.h>
#include <ctype.h>

/**
 * Returns the position of the first occurrence of the character in the string
 * */
int strpos(char* str, const char seek, int offset) {
    char* strPtr = str;
    int result = -1;
    int index = 0;
    while (*strPtr != '\0') {
        if (*strPtr == seek && result >= offset){
            result = index;
            return result;
        }
        ++index;
        ++strPtr;
    }
    return result;
}
