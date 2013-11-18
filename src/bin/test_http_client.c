#include <sys/socket.h>
#include <errno.h>     // errno
#include <string.h>    // strerror
#include <stdio.h>
#include <stdlib.h>    // atoi
#include <arpa/inet.h> // sockaddr_in, inet_pton, htons
#include <unistd.h>    // write, read, exit

// TODO: These util functions should be modularized.

int my_close(int file_desc) {
    int return_val;
    if ((return_val = close(file_desc)) == -1) {
        fprintf(stderr, "Could not close socket: %s.\n", strerror(errno));
        exit(1);
    }
    puts("The socket is closed.");
    return return_val;
}

int my_socket_stream() {
    int socket_desc = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        fprintf(stderr, "Could not create socket: %s.\n", strerror(errno));
        exit(1);
    }
    puts("The socket is opened.");
    return socket_desc;
}

void my_sockaddr_set_addr(struct sockaddr_in* addr_ptr, char* addr_str) {
    addr_ptr->sin_family = PF_INET;
    inet_pton(PF_INET, addr_str, &(addr_ptr->sin_addr));
}

void my_sockaddr_set_port(struct sockaddr_in* addr_ptr, int port) {
    addr_ptr->sin_port = htons(port);
}

void my_connect_addr_port(int socket_desc, char* addr_str, int port) {

    struct sockaddr_in remote_addr;
    my_sockaddr_set_addr(&remote_addr, addr_str);
    my_sockaddr_set_port(&remote_addr, port);

    if (connect(socket_desc, (struct sockaddr*) &remote_addr, sizeof remote_addr) == -1) {
        fprintf(stderr, "Could not connect to %s on port %d: %s.\n", addr_str, port, strerror(errno));
        my_close(socket_desc);
        exit(1);
    }
    printf("Connected to %s:%d.\n", addr_str, port);

}

// The my_sockaddr_set_port is easier to use.
#if 0
void my_sockaddr_get_addr(struct sockaddr_in* addr_ptr, char* addr_str, int addr_str_size) {
    inet_ntop(PF_INET, &(addr_ptr->sin_addr), addr_str, addr_str_size);
}

void my_sockaddr_get_port(struct sockaddr_in* addr_ptr, int* port_ptr) {
    *port_ptr = ntohs(addr_ptr->sin_port);
}

void my_connect(int socket_desc, struct sockaddr_in* addr_ptr) {

    char addr_str[INET_ADDRSTRLEN];
    my_sockaddr_get_addr(addr_ptr, addr_str, sizeof addr_str);
    int port;
    my_sockaddr_get_port(addr_ptr, &port);

    if (connect(socket_desc, (struct sockaddr*) addr_ptr, sizeof *addr_ptr) == -1) {
        fprintf(stderr, "Could not connect to %s on port %d: %s.\n", addr_str, port, strerror(errno));
        my_close(socket_desc);
        exit(1);
    }
    printf("Connected to %s:%d.\n", addr_str, port);

}
#endif

void my_send(int socket_desc, char* data, int data_size) {
    if (write(socket_desc, data, data_size) == -1) {
        fprintf(stderr, "Could not send data: %s.\n", strerror(errno));
        my_close(socket_desc);
        exit(1);
    }
    shutdown(socket_desc, SHUT_WR);
    puts("Sent data.");
}

void my_receive(int socket_desc) {
    int read_size;
    char buffer[1024];
    puts("--- Data received ---");
    while ((read_size = read(socket_desc, buffer, sizeof buffer))) {
        if (read_size == -1) {
            fprintf(stderr, "Could not read data: %s.\n", strerror(errno));
            my_close(socket_desc);
            exit(1);
        }
        write(STDOUT_FILENO, buffer, read_size);
    }
    if (buffer[read_size-1] != '\n') puts("");
    puts("--- End ---");
    shutdown(socket_desc, SHUT_RD);
}

int main(int argc, char* argv[]) {

    // Open stream socket:
    int socket_desc = my_socket_stream();

    // Get the port and addr settings from user:
    int port = 5000;
    if (argc >= 2) port = atoi(argv[1]);
    char* addr_str = "127.0.0.1";
    if (argc >= 3) addr_str = argv[2];

    // Connect to remote:
    my_connect_addr_port(socket_desc, addr_str, port);

    // Send data to remote:
    char data[] = "GET / HTTP/1.1\r\n\r\n";
    my_send(socket_desc, data, sizeof data);

    // Receive data from remote:
    my_receive(socket_desc);

    // Close the socket:
    my_close(socket_desc);

    exit(0);
}
