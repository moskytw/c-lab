#include <sys/socket.h>
#include <errno.h>     // errno
#include <string.h>    // strerror
#include <stdio.h>
#include <arpa/inet.h> // sockaddr_in, inet_pton, htons, inet_ntop, ntohs
#include <stdlib.h>    // exit
#include <unistd.h>    // STDOUT_FILENO, write, read

int sockutil_socket_stream() {
    int socket_desc = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        fprintf(stderr, "Could not create stream socket: %s.\n", strerror(errno));
        exit(1);
    }
    puts("Opend a socket.");
    return socket_desc;
}

int sockutil_socket_datagram() {
    int socket_desc = socket(PF_INET, SOCK_DGRAM, 0);
    if (socket_desc == -1) {
        fprintf(stderr, "Could not create datagram socket: %s.\n", strerror(errno));
        exit(1);
    }
    puts("The socket is opened.");
    return socket_desc;
}

void sockutil_sockaddr_set_addr(struct sockaddr_in* addr_ptr, char* addr_str) {
    addr_ptr->sin_family = PF_INET;
    inet_pton(PF_INET, addr_str, &(addr_ptr->sin_addr));
}

void sockutil_sockaddr_set_port(struct sockaddr_in* addr_ptr, int port) {
    addr_ptr->sin_port = htons(port);
}

void sockutil_bind_addr_port_retry(int bound_socket_desc, char* addr_str, int port, int retry_limit) {

    struct sockaddr_in bind_addr = {0};
    sockutil_sockaddr_set_addr(&bind_addr, addr_str);
    sockutil_sockaddr_set_port(&bind_addr, port);

    while (retry_limit--) {
        if (bind(bound_socket_desc, (struct sockaddr*) &bind_addr, (socklen_t) sizeof bind_addr) == -1) {
            if (errno == EADDRINUSE) {
                printf("The address %s port %d is in use. Try next port.\n", addr_str, port);
                port += 1;
                bind_addr.sin_port = htons(port);
                continue;
            }
            fprintf(stderr, "Could not find a unused port.\n");
            exit(1);
        }
        break;
    }
    printf("Bound to %s:%d.\n", addr_str, port);

}

void sockutil_listen(int bound_socket_desc) {
    if (listen(bound_socket_desc, 0) == -1) {
        fprintf(stderr, "Could not listen: %s.\n", strerror(errno));
        exit(1);
    }
    puts("Started to listen.");
}

void sockutil_sockaddr_get_addr(struct sockaddr_in* addr_ptr, char* addr_str, int addr_str_size) {
    inet_ntop(PF_INET, &(addr_ptr->sin_addr), addr_str, addr_str_size);
}

void sockutil_sockaddr_get_port(struct sockaddr_in* addr_ptr, int* port_ptr) {
    *port_ptr = ntohs(addr_ptr->sin_port);
}

int sockutil_accept_addr_port(int bound_socket_desc, char* addr_str, int addr_str_size, int* port_ptr) {

    struct sockaddr_in remote_addr = {0};
    socklen_t remote_addr_size = sizeof remote_addr;

    int remote_socket_desc;
    if ((remote_socket_desc = accept(bound_socket_desc, (struct sockaddr*) &remote_addr, &remote_addr_size)) == -1) {
        fprintf(stderr, "Could not accept: %s.\n", strerror(errno));
        exit(1);
    }
    puts("Accepted a connection as a new socket.");

    sockutil_sockaddr_get_addr(&remote_addr, addr_str, addr_str_size);
    sockutil_sockaddr_get_port(&remote_addr, port_ptr);

    return remote_socket_desc;
}

void sockutil_connect_addr_port(int socket_desc, char* addr_str, int port) {

    struct sockaddr_in remote_addr;
    sockutil_sockaddr_set_addr(&remote_addr, addr_str);
    sockutil_sockaddr_set_port(&remote_addr, port);

    if (connect(socket_desc, (struct sockaddr*) &remote_addr, sizeof remote_addr) == -1) {
        fprintf(stderr, "Could not connect to %s on port %d: %s.\n", addr_str, port, strerror(errno));
        exit(1);
    }
    printf("Connected to %s on port %d.\n", addr_str, port);

}

void sockutil_shutdown_write(int socket_desc) {
    if (shutdown(socket_desc, SHUT_WR) == -1) {
        fprintf(stderr, "Could not shut the write channel down: %s.\n", strerror(errno));
        exit(1);
    }
    puts("Shut the write channel down.");
}

void sockutil_send(int socket_desc, char* data, int data_size) {
    printf("Sending the data ... ");
    if (write(socket_desc, data, data_size) == -1) {
        fprintf(stderr, "Could not send data: %s.\n", strerror(errno));
        exit(1);
    }
    puts("Done.");
}

void sockutil_receive(int socket_desc) {

    int read_size;
    char buffer[1024];

    puts("Receiving the data ... ");
    puts("--- Data received ---");
    while ((read_size = read(socket_desc, buffer, sizeof buffer))) {
        if (read_size == -1) {
            fprintf(stderr, "Could not read data: %s.\n", strerror(errno));
            exit(1);
        }
        write(STDOUT_FILENO, buffer, read_size);
    }
    if (buffer[read_size-1] != '\n') puts("");
    puts("--- End ---");
    puts("Done.");
}

int sockutil_close(int file_desc) {
    int return_val;
    if ((return_val = close(file_desc)) == -1) {
        fprintf(stderr, "Could not close socket: %s.\n", strerror(errno));
        exit(1);
    }
    puts("Closed the socket.");
    return return_val;
}
