#include <string.h>

int atoi(const char *str, int* value) {
    int multiplier = 1;
    int accumulator = 0;
    while (*str != '\0') {
        int toAdd = 0;
        switch (str[0]){
            case '1':
                toAdd = 1; break;
            case '2':
                toAdd = 2; break;
            case '3':
                toAdd = 3; break;
            case '4':
                toAdd = 4; break;
            case '5':
                toAdd = 5; break;
            case '6':
                toAdd = 6; break;
            case '7':
                toAdd = 7; break;
            case '8':
                toAdd = 8; break;
            case '9':
                toAdd = 9; break;
            default:
                return -1;
        }
        accumulator = accumulator * multiplier + toAdd;
        multiplier *= 10;
        ++str; 
    }
    *value = accumulator;
    return 0;
}
