#ifndef _STRING_H
#define _STRING_H 1

//#include <errno.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

char* strcpy(char *restrict dest, const char *restrict src);

int memcmp(const void *left, const void *right, size_t n);

void* memcpy(void *restrict dest, const void *restrict src, size_t n);

void* memmove(void *dest, const void *src, size_t n);

void* memset(void *mem, int val, size_t n);

size_t strlen(const char *str);

int strpos(const char *str1, const char c, int offset);
// non-standard functions

// converts all chars [A-Za-z] to uppercase
char* strupper(char *str);

char* strlower(char *str);


#ifdef __cplusplus
}
#endif

#endif
