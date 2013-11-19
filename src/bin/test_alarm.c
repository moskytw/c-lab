/*
 * Use alarm to simulate the async IO.
 */

#include "termios_util.h"
#include <signal.h>
#include <fcntl.h>
#include <unistd.h> // STDIN_FILENO, ualarm
#include <stdio.h>

void my_sigalrm_handler(int signal) {
    puts("Hi, there!");
}

int main(int argc, char* argv[]) {

    signal(SIGALRM, my_sigalrm_handler);
    ualarm(500000, 500000); // send signal every 0.5s

    puts("It is annoying, right? Press 'y' to exit.");

    termios_util_disable_canonical_echo();
    while (getchar() != 'y');
    termios_util_restore();

    puts("Exit!");

    return 0;
}
