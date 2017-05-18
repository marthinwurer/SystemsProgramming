/*
** File: libc/string/memset.c
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors:
**
** Contains the memset function implementation
**
*/

#include <string.h>


void* memset(void *memPtr, int value, size_t n) {

    unsigned char *mem = (unsigned char *)memPtr;
    for (size_t i = 0; i != n; ++i) {
        mem[i] = (unsigned char)value;
    }

    return memPtr;

}
