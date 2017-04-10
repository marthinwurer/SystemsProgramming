
#include <string.h>
#include <ctype.h>


char* strupper(char *str) {
    char *strPtr = str;
    while (*strPtr != '\0') {
        *strPtr = toupper(*strPtr);
        ++strPtr;
    }

    return str;
}