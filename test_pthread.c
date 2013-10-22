#include <termios.h>
#include <unistd.h> // STDIN_FILENO, usleep, ...
#include <pthread.h>
#include <stdio.h>

struct termios my_original_termios;

void termios_set_non_canonical_mode() {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, &my_original_termios);
    new_termios = my_original_termios;
    new_termios.c_lflag &= ~ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

void termios_restore() {
    tcsetattr(STDIN_FILENO, TCSANOW, &my_original_termios);
}

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
    termios_set_non_canonical_mode();
    while (resp != 'y') {
        scanf("%c", &resp);
        puts("");
    }
    termios_restore();
    pthread_cancel(listener_thread);

    puts("exit!");
    pthread_exit(NULL);
}
