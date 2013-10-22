#include <termios.h>
#include <unistd.h> // STDIN_FILENO

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

