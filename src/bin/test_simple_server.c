#include <stdio.h>
#include <stdlib.h> // atoi, exit
#include "socket_util.h"

int main(int argc, char* argv[]) {

    // Get the port and addr settings:
    int bind_port = 5000;
    if (argc >= 2) bind_port = atoi(argv[1]);
    char* bind_addr_str_ptr = "0.0.0.0";
    if (argc >= 3) bind_addr_str_ptr = argv[2];

    // Open a socket for binding the address:
    int bind_socket_desc = socket_util_socket_stream();

    // Bind the socket with the address:
    socket_util_bind_addr_port_retry(
        bind_socket_desc, bind_addr_str_ptr, bind_port, 3
    );

    // Listen for connection:
    socket_util_listen(bind_socket_desc);

    // Be server forever:
    while(1) {

        puts("\nWaiting for a connection ...");

        // Accept for a connection as a new socket:
        char remote_addr_str[INET_ADDRSTRLEN];
        int remote_port;
        int remote_socket_desc = socket_util_accept_addr_port(
            bind_socket_desc,
            remote_addr_str, sizeof remote_addr_str, &remote_port
        );
        printf("The remote address %s port %d is connected.\n",
            remote_addr_str, remote_port
        );

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

    // Close bind socket:
    printf("Closing bind socket: ");
    socket_util_close(bind_socket_desc);

    exit(0);
}
