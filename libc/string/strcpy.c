/*
** File: libc/string/strcpy.c
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors:
**
** Contains the strcpy function implementation
**
*/

#include <string.h>


char* strcpy(char *restrict dest, const char *restrict src) {

    char *destPtr = dest;
    do {
        *destPtr++ = *src++;
    } while (*src != '\0');
    
    return dest;
}
