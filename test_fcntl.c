#include "termios_util.h"
#include <fcntl.h>
#include <unistd.h> // usleep, STDIN_FILENO
#include <stdio.h>

int main(int argc, char* argv[]) {

    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

    puts("It is annoying, right? Press 'y' to exit.");

    termios_disable_canonical_echo();
    while (getchar() != 'y') {
        puts("Hi, there!");
        usleep(500000); // 0.5s
    }
    termios_restore();

    puts("Exit!");

    return 0;
}
