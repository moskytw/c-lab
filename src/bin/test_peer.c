#include <stdio.h>
#include <stdlib.h>    // atoi, exit
#include "socket_util.h"
#include <sys/socket.h>
#include <arpa/inet.h> // sockaddr_in
#include <errno.h>     // errno
#include <string.h>    // strerror
#include <unistd.h>    // STDOUT_FILENO, write, read

int main(int argc, char* argv[]) {

    // Get the port and addr settings:
    int port = 5000;
    if (argc >= 2) port = atoi(argv[1]);
    char* addr_str = "0.0.0.0";
    if (argc >= 3) addr_str = argv[2];

    // Open a socket for binding the address:
    int bound_socket_desc = sockutil_socket_datagram();

    // Bind the socket with the address:
    sockutil_bind_addr_port_retry(bound_socket_desc, addr_str, port, 3);

    int read_size;
    char buffer[1024];
    struct sockaddr_in remote_addr = {0};
    int remote_addr_size = sizeof remote_addr;
    puts("Receiving the data ... ");
    puts("--- Data received ---");
    while ((read_size = recvfrom(bound_socket_desc, buffer, sizeof buffer, 0, (struct sockaddr*) &remote_addr, (socklen_t*) &remote_addr_size))) {
        if (read_size == -1) {
            fprintf(stderr, "Could not read data: %s.\n", strerror(errno));
            exit(1);
        }
        write(STDOUT_FILENO, buffer, read_size);
    }
    puts("--- End ---");
    puts("Done.");

    // Close bound socket:
    printf("Closing bound socket: ");
    sockutil_close(bound_socket_desc);

    exit(0);
}
