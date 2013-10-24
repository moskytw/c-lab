/*
 * The non-blocing IO using fcntl.
 *
 * This implementation still has delay between press key and exit.
 */

#include "termios_util.h"
#include <fcntl.h>
#include <unistd.h> // STDIN_FILENO, usleep
#include <stdio.h>

int main(int argc, char* argv[]) {

    if (fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK) == -1) return 1;

    puts("It is annoying, right? Press 'y' to exit.");

    termios_disable_canonical_echo();
    while (getchar() != 'y') {
        puts("Hi, there!");
        // It always needs to sleep 0.5s.
        usleep(500000); // 0.5s
    }
    termios_restore();

    puts("Exit!");

    return 0;
}
