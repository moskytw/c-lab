/*
 * It implements some of the ANSI escape code.
 *
 * Ref: http://en.wikipedia.org/wiki/ANSI_escape_code
 */

#include <stdio.h>

void move_cursor_up(int n) {
    printf("\e[%dA", n);
}

void move_cursor_down(int n) {
    printf("\e[%dB", n);
}

void move_cursor_forward(int n) {
    printf("\e[%dC", n);
}

void move_cursor_back(int n) {
    printf("\e[%dD", n);
}

void earse_display(int n) {
    printf("\e[%dJ", n);
}

void earse_entire_display() {
    earse_display(2);
}

void earse_in_line(int n) {
    printf("\e[%dK", n);
}

void earse_entire_line() {
    earse_in_line(2);
}

void set_cursor_position(int n, int m) {
    printf("\e[%d;%dH", n, m);
}

void reset_cursor_position() {
    set_cursor_position(0, 0);
}
