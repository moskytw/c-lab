/*
 * Use multi-thread (pthread) to simulate async IO.
 */

#include "termios_util.h"
#include <pthread.h>
#include <unistd.h> // usleep
#include <stdio.h>

void* my_infinitely_repeater(void* tid) {

    while (1) {
        puts("Hi, main!");
        usleep(500000); // 0.5s
    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {

    pthread_t infinitely_repeater_thread;
    int tid = 0;
    int return_code = pthread_create(&infinitely_repeater_thread, NULL, my_infinitely_repeater, (void*) &tid);
    if (return_code) {
        fprintf(stderr, "Can't create thread. pthread_create() returned %d.\n", return_code);
        return 1;
    }

    puts("It is annoying, right? Press 'y' to exit.");
    termios_disable_canonical_echo();
    while (getchar() != 'y');
    termios_restore();

    pthread_cancel(infinitely_repeater_thread);

    puts("Exit!");
    pthread_exit(NULL);
}
