#include <stdio.h>

void debug_util_hexdump(void* ptr, int size) {

    fprintf(stderr, "HEXDUMP: ");

    if (ptr == NULL) {
        fprintf(stderr, "NULL");
    } else {
        int i;
        for (i = 0; i < size; i++) fprintf(stderr, "%X ", *((int*) (ptr+i)));
    }

    fprintf(stderr, "\n");

}

