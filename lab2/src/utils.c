#include "utils.h"
#include <stdint.h>

int cmp_string(const char *a, const char *b) {
    while(*a && (*a==*b)) {
        a++;
        b++;
    }
    return *(unsigned char*)a - *(unsigned char*)b;
}

unsigned int hex_to_dec(char *str) {
    int i = 0;
    unsigned int res = 0;
    while (str[i] != '\0') {
        char c = str[i];
        unsigned int val = 0;
        if (c>='0' && c<='9') {
            val = c - '0';
        } else if (c>='A' && c<='F') {
            val = c - 'A' + 10;
        }
        res = res*16 + val;
        i++;
    }
    return res;
}

uintptr_t alignup(uintptr_t tar, unsigned int num) {
    uintptr_t res = (tar+num)&(~(uintptr_t)num);
    return res;
}