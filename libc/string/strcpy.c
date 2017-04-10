
#include <string.h>


char* strcpy(char *restrict dest, const char *restrict src) {

    char *destPtr = dest;
    do {
        *destPtr++ = *src++;
    } while (*src != '\0');
    
    return dest;
}