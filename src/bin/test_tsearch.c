#include <stdio.h>
#include <stdlib.h> // free
#include <search.h>

int int_ptr_cmp(const void* a_ptr, const void* b_ptr) {
    return *(int*)a_ptr-*(int*)b_ptr;
}

void print_node_ptr(const void* node_ptr, VISIT order, int level) {
    if (order == postorder || order == leaf) {
        printf("node_ptr: %p value: %d level: %d\n", node_ptr, **(int**) node_ptr, level);
    }
}

int main(int argc, char* argv[]) {

    const int size = 5;
    int values[size] = {1, 2, 3, 4, 5};
    void* root = NULL;

    int i;
    for (i = 0; i < size; i++) {
        // This binary tree actually saves the pointer of the value.
        tsearch((void*) &values[i], &root, int_ptr_cmp);
    }

    int target = 1;
    printf("Find %d: %p\n", target,
        tfind((void*) &target, &root, int_ptr_cmp)
    );

    tdelete((void*) &target, &root, int_ptr_cmp);
    printf("Find %d: %p\n", target,
        tfind((void*) &target, &root, int_ptr_cmp)
    );

    twalk(root, print_node_ptr);
    // TODO: ensure it is enough to free this free.
    free(root);
}
