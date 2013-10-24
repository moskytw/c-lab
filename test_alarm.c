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

    if (fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK) == -1) return 1;

    signal(SIGALRM, my_sigalrm_handler);
    ualarm(500000, 500000);

    puts("It is annoying, right? Press 'y' to exit.");

    termios_disable_canonical_echo();
    while (getchar() != 'y');
    termios_restore();

    puts("Exit!");

    return 0;
}
