#include "ansi_esc_seq.h"
#include <stdio.h>

int main(int argc, char *argv[]) {

    earse_entire_display();
    reset_cursor_position();
    printf("Hello, World!");

    move_cursor_back(6);
    printf("Mosky");
    puts("");

    return 0;
}
