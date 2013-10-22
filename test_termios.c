#include "termios_util.h"
#include <stdio.h>

int main(int argc, char* argv[]) {

    puts("Press 'e' to leave.");

    termios_disable_canonical_echo();
    while (getchar() != 'e');
    termios_restore();

    puts("Exit!");

    return 0;
}

