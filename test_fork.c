#include "termios_util.h"
#include <unistd.h> // pid_t, fork, perror
#include <stdio.h>
#include <signal.h> // kill, SIGTERM

int main(int argc, char* argv[]) {

    pid_t child_pid = fork();
    if (child_pid < 0) {

        perror("Can't fork.");

    } else if (child_pid == 0) {

        while (1) {
            puts("Hi, there!");
            usleep(500000); // 0.5s
        }

    } else {

        puts("It is annoying, right? Press 'y' to exit.");
        termios_disable_canonical_echo();
        while (getchar() != 'y');
        termios_restore();
        kill(child_pid, SIGTERM);

    }

    puts("Exit!");

    return 0;
}
