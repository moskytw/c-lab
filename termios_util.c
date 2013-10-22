#include <termios.h>
#include <unistd.h> // STDIN_FILENO

struct termios termios_original_attr;

int termios_disable_canonical_echo() {
    struct termios termios_attr;
    int return_code = tcgetattr(STDIN_FILENO, &termios_original_attr);
    if (return_code) {
        return return_code;
    }
    termios_attr = termios_original_attr;
    termios_attr.c_lflag &= ~ICANON;
    termios_attr.c_lflag &= ~ECHO;
    return tcsetattr(STDIN_FILENO, TCSANOW, &termios_attr);
}

int termios_restore() {
    return tcsetattr(STDIN_FILENO, TCSANOW, &termios_original_attr);
}
