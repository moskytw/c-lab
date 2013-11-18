#include <stdio.h>

void debugutil_dump_hex(void* ptr, int size) {

    fprintf(stderr, "HEXDUMP: ");

    if (ptr == NULL) {
        fprintf(stderr, "NULL");
    } else {
        int i;
        for (i = 0; i < size; i++) fprintf(stderr, "%X ", *((int*) (ptr+i)));
    }

    fprintf(stderr, "\n");

}

