/*
 * The util helps you to disable the line buffer and the echo.
 */

#include <termios.h>
#include <unistd.h> // STDIN_FILENO

struct termios termios_util_original_attr;

int termios_util_disable_canonical_echo() {

    struct termios attr;
    int return_code = tcgetattr(STDIN_FILENO, &termios_util_original_attr);

    if (return_code) return return_code;

    attr = termios_util_original_attr;
    attr.c_lflag &= ~ICANON;
    attr.c_lflag &= ~ECHO;

    return tcsetattr(STDIN_FILENO, TCSANOW, &attr);
}

int termios_util_restore() {
    return tcsetattr(STDIN_FILENO, TCSANOW, &termios_util_original_attr);
}
