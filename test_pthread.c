#include <unistd.h> // STDIN_FILENO, usleep, ...
#include <pthread.h>
#include <stdio.h>

void* my_listener(void* listener_id) {

    while (1) {
        puts("Hi, main!");
        usleep(500000); // 0.5s
    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {

    pthread_t listener_thread;
    int listener_id = 0;
    int return_code = pthread_create(&listener_thread, NULL, my_listener, (void*) &listener_id);
    if (return_code) {
        fprintf(stderr, "Can't create thread. pthread_create() returned %d.", return_code);
        return 1;
    }

    puts("It is annoying, right? Say 'y' to exit.");
    char resp = 'n';
    while (resp != 'y') {
        scanf("%c", &resp);
    }
    pthread_cancel(listener_thread);

    puts("exit!");
    pthread_exit(NULL);
}
