#include "termios_util.h"
#include <fcntl.h>
#include <unistd.h> // usleep
#include <stdio.h>

int main(int argc, char* argv[]) {

    puts("It is annoying, right? Press 'y' to exit.");

    termios_disable_canonical_echo();
    while (1) {
        puts("Hi, there!");
        usleep(500000); // 0.5s
        if (getchar() == 'y') break;
    }
    termios_restore();

    puts("Exit!");

    return 0;
}
