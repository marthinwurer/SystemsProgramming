/*
** File: libc/string/strlen.c
**
** Author: Brennan Ringey (bjr1251)
**
** Contributors:
**
** Contains the strlen function implementation
**
*/

#include <string.h>

size_t strlen(const char *str) {

    size_t len = 0;
    while (*str != '\0') {
        ++len;
        ++str;
    }
    return len;

}
