#include <stdio.h>
#include "ansi_esc_seq.h"

int main(int argc, const char *argv[]) {

    earse_entire_display();
    reset_cursor_position();
    printf("Hello, World!");

    move_cursor_back(6);
    printf("Mosky");

    puts("");

    return 0;
}
