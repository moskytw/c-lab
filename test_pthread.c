#include <termios.h>
#include <unistd.h> // STDIN_FILENO, usleep, ...
#include <pthread.h>
#include <stdio.h>

struct termios termios_original_attr;

void termios_disable_canonical_echo() {
    struct termios termios_attr;
    tcgetattr(STDIN_FILENO, &termios_original_attr);
    termios_attr = termios_original_attr;
    termios_attr.c_lflag &= ~ICANON;
    termios_attr.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &termios_attr);
}

void termios_restore() {
    tcsetattr(STDIN_FILENO, TCSANOW, &termios_original_attr);
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

    puts("It is annoying, right? Press 'y' to exit.");
    char resp = 'n';
    termios_disable_canonical_echo();
    while (resp != 'y') {
        scanf("%c", &resp);
    }
    termios_restore();
    pthread_cancel(listener_thread);

    puts("Exit!");
    pthread_exit(NULL);

    return 0;
}
