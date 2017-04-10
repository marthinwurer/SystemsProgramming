#include <string.h>


void* memmove(void *dest, const void *src, size_t n) {

    unsigned char *dstPtr = (unsigned char*)dest;
    const unsigned char *srcPtr = (const unsigned char*)src;

    if (dstPtr < srcPtr) {
        for (size_t i = 0; i != n; ++i) {
            dstPtr[i] = srcPtr[i];
        }
    } else {
        for (size_t i = n; i != 0; --i) {
            dstPtr[i - 1] = srcPtr[i - 1];
        }
    }

    return dstPtr;

}
