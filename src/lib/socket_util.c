#include <sys/socket.h>
#include <errno.h>     // errno
#include <string.h>    // strerror
#include <stdio.h>
#include <arpa/inet.h> // sockaddr_in, inet_pton, htons, inet_ntop, ntohs
#include <stdlib.h>    // exit
#include <unistd.h>    // STDOUT_FILENO, write, read

int socket_util_socket_stream() {
    int socket_desc = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        fprintf(stderr, "Could not create stream socket: %s.\n",
            strerror(errno)
        );
        exit(1);
    }
    puts("Opend a socket.");
    return socket_desc;
}

int socket_util_socket_datagram() {
    int socket_desc = socket(PF_INET, SOCK_DGRAM, 0);
    if (socket_desc == -1) {
        fprintf(stderr, "Could not create datagram socket: %s.\n",
            strerror(errno)
        );
        exit(1);
    }
    puts("The socket is opened.");
    return socket_desc;
}

void socket_util_sockaddr_set_addr(struct sockaddr_in* addr_ptr, char* addr_str_ptr) {
    addr_ptr->sin_family = PF_INET;
    inet_pton(PF_INET, addr_str_ptr, &(addr_ptr->sin_addr));
}

void socket_util_sockaddr_set_port(struct sockaddr_in* addr_ptr, int port) {
    addr_ptr->sin_port = htons(port);
}

int socket_util_bind_addr_port_retry(int bind_socket_desc, char* bind_addr_str_ptr, int bind_port, int retry_limit) {

    struct sockaddr_in bind_addr = {0};
    socket_util_sockaddr_set_addr(&bind_addr, bind_addr_str_ptr);
    socket_util_sockaddr_set_port(&bind_addr, bind_port);

    while (retry_limit--) {

        if (bind(bind_socket_desc, (struct sockaddr*) &bind_addr, (socklen_t) sizeof bind_addr) == -1) {
            if (errno == EADDRINUSE) {
                printf("The address %s port %d is in use. Try next port.\n",
                    bind_addr_str_ptr, bind_port
                );
                bind_port += 1;
                bind_addr.sin_port = htons(bind_port);
                continue;
            }
            fprintf(stderr, "Could not find a unused port.\n");
            exit(1);
        }
        break;
    }
    printf("Bound to %s:%d.\n", bind_addr_str_ptr, bind_port);

    return bind_port;
}

void socket_util_listen(int bind_socket_desc) {
    if (listen(bind_socket_desc, 0) == -1) {
        fprintf(stderr, "Could not listen: %s.\n", strerror(errno));
        exit(1);
    }
    puts("Started to listen.");
}

void socket_util_sockaddr_get_addr(struct sockaddr_in* addr_ptr, char* addr_str_ptr, int addr_str_size) {
    inet_ntop(PF_INET, &(addr_ptr->sin_addr), addr_str_ptr, addr_str_size);
}

void socket_util_sockaddr_get_port(struct sockaddr_in* addr_ptr, int* port_ptr) {
    *port_ptr = ntohs(addr_ptr->sin_port);
}

int socket_util_accept_addr_port(int bind_socket_desc, char* remote_addr_str_ptr, int remote_addr_str_size, int* remote_port_ptr) {

    struct sockaddr_in remote_addr = {0};
    socklen_t remote_addr_size = sizeof remote_addr;

    int remote_socket_desc;
    if ((remote_socket_desc = accept(bind_socket_desc, (struct sockaddr*) &remote_addr, &remote_addr_size)) == -1) {
        fprintf(stderr, "Could not accept: %s.\n", strerror(errno));
        exit(1);
    }
    puts("Accepted a connection as a new socket.");

    socket_util_sockaddr_get_addr(&remote_addr, remote_addr_str_ptr, remote_addr_str_size);
    socket_util_sockaddr_get_port(&remote_addr, remote_port_ptr);

    return remote_socket_desc;
}

void socket_util_connect_addr_port(int socket_desc, char* remote_addr_str_ptr, int remote_port) {

    struct sockaddr_in remote_addr;
    socket_util_sockaddr_set_addr(&remote_addr, remote_addr_str_ptr);
    socket_util_sockaddr_set_port(&remote_addr, remote_port);

    if (connect(socket_desc, (struct sockaddr*) &remote_addr, sizeof remote_addr) == -1) {
        fprintf(stderr, "Could not connect to %s on port %d: %s.\n", remote_addr_str_ptr, remote_port, strerror(errno));
        exit(1);
    }
    printf("Connected to %s on port %d.\n", remote_addr_str_ptr, remote_port);

}

void socket_util_shutdown_write(int socket_desc) {
    if (shutdown(socket_desc, SHUT_WR) == -1) {
        fprintf(stderr, "Could not shut the write channel down: %s.\n", strerror(errno));
        exit(1);
    }
    puts("Shut the write channel down.");
}

void socket_util_send(int socket_desc, char* data_ptr, int data_size) {
    printf("Sending the data ... ");
    if (write(socket_desc, data_ptr, data_size) == -1) {
        fprintf(stderr, "Could not send data: %s.\n", strerror(errno));
        exit(1);
    }
    puts("Done.");
}

void socket_util_sendto(int socket_desc, struct sockaddr_in* remote_addr_ptr, char* data_ptr, int data_size) {
    if (sendto(socket_desc, data_ptr, data_size, 0, (struct sockaddr*) remote_addr_ptr, sizeof *remote_addr_ptr) == -1) {
        fprintf(stderr, "Could not send data: %s.\n", strerror(errno));
        exit(1);
    }
}

void socket_util_receive(int socket_desc) {

    int read_size;
    char buffer[1024];

    puts("Receiving the data ... ");
    puts("--- Data received ---");
    while ((read_size = read(socket_desc, buffer, sizeof buffer))) {
        if (read_size == -1) {
            fprintf(stderr, "Could not read data: %s.\n", strerror(errno));
            exit(1);
        }
        if (write(STDOUT_FILENO, buffer, read_size) == -1) {
            fprintf(stderr, "Could not write data: %s.\n", strerror(errno));
            exit(1);
        }
    }
    if (read_size >= 1 && buffer[read_size-1] != '\n') puts("");
    puts("--- End ---");
    puts("Done.");
}

void socket_util_close(int file_desc) {
    if (close(file_desc) == -1) {
        fprintf(stderr, "Could not close socket: %s.\n", strerror(errno));
        exit(1);
    }
    puts("Closed the socket.");
}
