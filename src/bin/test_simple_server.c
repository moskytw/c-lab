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

    // Open socket:
    int bind_socket_desc = my_socket_stream();

    // Get the port and addr settings from user:
    int port = 5000;
    if (argc >= 2) port = atoi(argv[1]);
    char* addr_str = "0.0.0.0";
    if (argc >= 3) addr_str = argv[2];

    // Bind socket with this address:
    struct sockaddr_in bind_addr = {0};
    my_sockaddr_set_addr(&bind_addr, addr_str);
    my_sockaddr_set_port(&bind_addr, port);
    int try_limit = 3;
    while (try_limit--) {
        if (bind(bind_socket_desc, (struct sockaddr*) &bind_addr, (socklen_t) sizeof bind_addr) == -1) {
            if (errno == EADDRINUSE) {
                printf("The address %s on port %d is in use. Try next port.\n", addr_str, port);
                port += 1;
                bind_addr.sin_port = htons(port);
                continue;
            }
            fprintf(stderr, "Could not find a unused port.\n");
            my_close(bind_socket_desc);
            exit(1);
        }
        break;
    }
    printf("Bound to %s:%d.\n", addr_str, port);

    // Listen for a connection from remote:
    if (listen(bind_socket_desc, 0) == -1) {
        fprintf(stderr, "Could not listen: %s.\n", strerror(errno));
        my_close(bind_socket_desc);
        exit(1);
    }
    puts("Listening ...");

    // Accept for a connection from remote:
    struct sockaddr_in remote_addr = {0};
    socklen_t remote_addr_size = sizeof remote_addr;
    int accept_socket_desc;
    if ((accept_socket_desc = accept(bind_socket_desc, (struct sockaddr*) &remote_addr, &remote_addr_size)) == -1) {
        fprintf(stderr, "Could not accept: %s.\n", strerror(errno));
        my_close(bind_socket_desc);
        exit(1);
    }
    puts("Accepted a connection as a socket.");

    // Receive data from remote:
    my_receive(accept_socket_desc);

    // Send data to remote:
    char data[] = "OK";
    my_send(accept_socket_desc, data, sizeof data);

    // Close the sockets:
    printf("Close accept socket: ");
    my_close(accept_socket_desc);
    printf("Close bind socket: ");
    my_close(bind_socket_desc);

    exit(0);
}
