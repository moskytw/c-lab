#include <stdio.h>

int main(int argc, char* argv[]) {

    char str[] = "I am string.";
    char* str_ptr = str;

    printf("sizeof str (%s) is %lu.\n", str, sizeof str);
    printf("sizeof str_ptr (%s) is %lu.\n", str_ptr, sizeof str_ptr);
    printf("sizeof *str_ptr (%c) is %lu.\n", *str_ptr, sizeof *str_ptr);

    return 0;
}
