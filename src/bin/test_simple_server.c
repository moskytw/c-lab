#include <stdio.h>
#include <stdlib.h> // atoi, exit
#include "socket_util.h"

int main(int argc, char* argv[]) {

    // Get the port and addr settings:
    int port = 5000;
    if (argc >= 2) port = atoi(argv[1]);
    char* addr_str = "0.0.0.0";
    if (argc >= 3) addr_str = argv[2];

    // Open a socket for binding the address:
    int bound_socket_desc = socket_util_socket_stream();

    // Bind the socket with the address:
    socket_util_bind_addr_port_retry(bound_socket_desc, addr_str, port, 3);

    // Listen for connection:
    socket_util_listen(bound_socket_desc);

    // Be server forever:
    while(1) {

        puts("\nWaiting for a connection ...");

        // Accept for a connection as a new socket:
        char addr_str[INET_ADDRSTRLEN];
        int port;
        int remote_socket_desc = socket_util_accept_addr_port(bound_socket_desc, addr_str, sizeof addr_str, &port);
        printf("The remote address %s port %d is connected.\n", addr_str, port);

        // Receive data:
        socket_util_receive(remote_socket_desc);
        // Once this function returns, the remote closed his write channel.

        // Send data:
        char data[] = "OK";
        socket_util_send(remote_socket_desc, data, sizeof data);

        // Close remote socket:
        printf("Closing remote socket: ");
        socket_util_close(remote_socket_desc);
    }

    // Close bound socket:
    printf("Closing bound socket: ");
    socket_util_close(bound_socket_desc);

    exit(0);
}
