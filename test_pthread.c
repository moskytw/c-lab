#include "termios_util.h"
#include <pthread.h>
#include <unistd.h> // usleep
#include <stdio.h>

void* my_infinitely_repeat(void* tid) {

    while (1) {
        puts("Hi, main!");
        usleep(500000); // 0.5s
    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {

    pthread_t infinitely_repeat_thread;
    int tid = 0;
    int return_code = pthread_create(&infinitely_repeat_thread, NULL, my_infinitely_repeat, (void*) &tid);
    if (return_code) {
        fprintf(stderr, "Can't create thread. pthread_create() returned %d.", return_code);
        return 1;
    }

    puts("It is annoying, right? Press 'y' to exit.");
    termios_disable_canonical_echo();
    while (getchar() != 'y');
    termios_restore();
    pthread_cancel(infinitely_repeat_thread);

    puts("Exit!");
    pthread_exit(NULL);

    return 0;
}
