#include <stdlib.h> // atoi, exit
#include "socket_util.h"

int main(int argc, char* argv[]) {

    // Get the port and addr settings:
    int port = 5000;
    if (argc >= 2) port = atoi(argv[1]);
    char* addr_str = "127.0.0.1";
    if (argc >= 3) addr_str = argv[2];

    // Open stream socket for connecting the address:
    int socket_desc = sockutil_socket_stream();

    // Connect the address using the socket:
    sockutil_connect_addr_port(socket_desc, addr_str, port);

    // Send data:
    char data[] = "GET / HTTP/1.1\r\n\r\n";
    sockutil_send(socket_desc, data, sizeof data);

    // Tell the remote the data are all sent:
    sockutil_shutdown_write(socket_desc);

    // Receive data:
    sockutil_receive(socket_desc);

    // Close the socket:
    sockutil_close(socket_desc);

    exit(0);
}
