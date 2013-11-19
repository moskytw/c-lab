#include "socket_util.h"
#include <stdio.h>
#include <stdlib.h>    // atoi, exit, malloc, free
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h> // sockaddr_in
#include <errno.h>     // errno
#include <string.h>    // strerror
#include <unistd.h>    // STDOUT_FILENO, write, read

// TODO: Define the control message.

const int broadcast_addr_ptrs_size = 10;
struct sockaddr_in* broadcast_addr_ptrs[broadcast_addr_ptrs_size] = {0};
pthread_mutex_t broadcast_addr_ptrs_mutex;

void my_broadcast_addr_ptrs_add(struct sockaddr_in* remote_addr_ptr) {

    int i;
    int empty_idx = -1;

    for (i = 0; i < broadcast_addr_ptrs_size; i++) {

        // do noting if this addr is existent
        // TODO: Use binary tree or hash table to do it.
        if (broadcast_addr_ptrs[i] != NULL &&
            !memcmp(broadcast_addr_ptrs[i], remote_addr_ptr, sizeof (struct sockaddr_in))) {
            return;
        }

        // find an empty index
        if (empty_idx == -1 && broadcast_addr_ptrs[i] == NULL) {
            empty_idx = i;
        }

    }

    if (empty_idx != -1) {
        broadcast_addr_ptrs[empty_idx] = malloc(sizeof (struct sockaddr_in));
        *broadcast_addr_ptrs[empty_idx] = *remote_addr_ptr;
        puts("It is a new remote!");
    }
}

void my_broadcast_addr_ptrs_erase(int i) {
    free(broadcast_addr_ptrs[i]);
    broadcast_addr_ptrs[i] = NULL;
}

int bind_socket_desc;

void* my_receiver() {

    int read_size;
    char buffer[1024];
    struct sockaddr_in remote_addr = {0};
    int remote_addr_size = sizeof remote_addr;

    while ((read_size = recvfrom(bind_socket_desc, buffer, sizeof buffer, 0, (struct sockaddr*) &remote_addr, (socklen_t*) &remote_addr_size))) {

        if (read_size == -1) {
            fprintf(stderr, "Could not read data: %s.\n", strerror(errno));
            pthread_exit(NULL);
        }

        pthread_mutex_lock(&broadcast_addr_ptrs_mutex);
        my_broadcast_addr_ptrs_add(&remote_addr);
        pthread_mutex_unlock(&broadcast_addr_ptrs_mutex);

        char remote_addr_str[INET_ADDRSTRLEN];
        int remote_port;
        socket_util_sockaddr_get_addr(&remote_addr, remote_addr_str, sizeof remote_addr_str);
        socket_util_sockaddr_get_port(&remote_addr, &remote_port);

        // TODO: This message should be sent to all broadcast addrs except this
        // remote.
        printf("[%s:%d] ", remote_addr_str, remote_port);
        fflush(stdout);
        write(STDOUT_FILENO, buffer, read_size);

    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {

    // Get the port and addr settings:
    int bind_port = 5000;
    if (argc >= 2) bind_port = atoi(argv[1]);
    char* bind_addr_str_ptr = "0.0.0.0";
    if (argc >= 3) bind_addr_str_ptr = argv[2];

    // Open a socket for binding the address:
    bind_socket_desc = socket_util_socket_datagram();

    // Bind the socket with the address:
    socket_util_bind_addr_port_retry(bind_socket_desc, bind_addr_str_ptr, bind_port, 3);

    // Create thread to receive data in background:
    pthread_t my_receiver_thread;
    if (pthread_create(&my_receiver_thread, NULL, my_receiver, NULL)) {
        fprintf(stderr, "Could not create thread: %s\n", strerror(errno));
        exit(1);
    }

    // Accept message from user:
    while (1) {
        char buffer[1024];
        fgets(buffer, sizeof buffer, stdin);
        // TODO: send message to all broadcast addrs
        if (!strncmp(buffer, "exit", 4)) {
            exit(0);
        }
    }

    // Close bind socket:
    printf("Closing bind socket: ");
    socket_util_close(bind_socket_desc);

    exit(0);
}