#include <string.h>

int memcmp(const void *leftPtr, const void *rightPtr, size_t n) {
    const unsigned char *left = (const unsigned char*)leftPtr;
    const unsigned char *right = (const unsigned char*)rightPtr;
    
    for (size_t i = 0; i != n; ++i) {
        unsigned char a = left[i], b = right[i];

        if (a == b) {
            continue;
        } else if (a < b) {
            return -1;
        } else {
            return 1;
        }
    }
    return 0;
}