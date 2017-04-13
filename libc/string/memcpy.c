#include <string.h>

void* memcpy(void *restrict destptr, const void *restrict srcptr, size_t n) {
    unsigned char *dest = (unsigned char *)destptr;
    const unsigned char *src = (const unsigned char *)srcptr;
    
    for (size_t i = 0; i != n; i++) {
        dest[i] = src[i];
    }

    return destptr;
}