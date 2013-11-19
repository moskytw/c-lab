#include <stdio.h>
#include <string.h>

struct node {
    char value[128];
    struct node* next;
};

int main(int argc, char* argv[]) {

    int n = 1234;
    int* n_ptr = &n;

    struct node my_node = {
        .value = "I am the struct node.",
        .next  = NULL
    };
    struct node* my_node_ptr = &my_node;

    char str[] = "I am string.";
    char* str_ptr = str;

    printf("sizeof n (%d) is %lu.\n", n, sizeof n);
    // -> 4
    printf("sizeof *n_ptr (%d) is %lu.\n", *n_ptr, sizeof *n_ptr);
    // -> 4
    printf("sizeof n_ptr (%p) is %lu.\n", n_ptr, sizeof n_ptr);
    // -> 8

    printf("sizeof my_node is %lu.\n", sizeof my_node);
    // -> 136 (128+8)
    printf("sizeof *my_node_ptr is %lu.\n", sizeof *my_node_ptr);
    // -> 136
    printf("sizeof my_node_ptr (%p) is %lu.\n", my_node_ptr, sizeof my_node_ptr);
    // -> 8
    // struct works as the normal type

    printf("sizeof str (%s) is %lu.\n", str, sizeof str);
    // -> 13
    printf("sizeof *str_ptr (%c) is %lu.\n", *str_ptr, sizeof *str_ptr);
    // -> 1
    // It is a char point which points to the first char of this string.
    printf("len(str_ptr) (%s) is %lu.\n", str_ptr, strlen(str_ptr));
    // -> 12
    // without including the terminating null character itself.
    printf("sizeof str_ptr (%p) is %lu.\n", str_ptr, sizeof str_ptr);
    // -> 8

    return 0;
}
