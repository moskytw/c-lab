/*
 * This is a version improved from test_fcntl.c by using O_ASYNC.
 *
 * It resolved the delay between pressing 'y' and exit. But it still has a
 * detect, that is it prints "Hi, there!" immediately when you press any other
 * key.
 */

#include "termios_util.h"
#include <fcntl.h>
#include <signal.h>
#include <unistd.h> // STDIN_FILENO, usleep
#include <stdio.h>

void my_null_handler(int signal) {}

int main(int argc, char* argv[]) {

    // added O_ASYNC to make os send SIGIO
    if (fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK|O_ASYNC) == -1) return 1;

    puts("It is annoying, right? Press 'y' to exit.");

    // make process don't ignore it
    signal(SIGIO, my_null_handler);

    termios_disable_canonical_echo();
    while (getchar() != 'y') {
        puts("Hi, there!");
        // process will wake up when we get the signal
        usleep(500000); // 0.5s
    }
    termios_restore();

    puts("Exit!");

    return 0;
}
