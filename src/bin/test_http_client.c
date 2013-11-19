#include <stdlib.h> // atoi, exit
#include "socket_util.h"

int main(int argc, char* argv[]) {

    // Get the port and addr settings:
    int remote_port = 5000;
    if (argc >= 2) remote_port = atoi(argv[1]);
    char* remote_addr_str_ptr = "127.0.0.1";
    if (argc >= 3) remote_addr_str_ptr = argv[2];

    // Open stream socket for connecting the address:
    int remote_socket_desc = socket_util_socket_stream();

    // Connect the address using the socket:
    socket_util_connect_addr_port(
        remote_socket_desc, remote_addr_str_ptr, remote_port
    );

    // Send data:
    char data[] = "GET / HTTP/1.1\r\n\r\n";
    socket_util_send(remote_socket_desc, data, sizeof data);

    // Tell the remote the data are all sent:
    socket_util_shutdown_write(remote_socket_desc);

    // Receive data:
    socket_util_receive(remote_socket_desc);

    // Close the socket:
    socket_util_close(remote_socket_desc);

    exit(0);
}
