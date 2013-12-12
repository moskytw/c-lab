#include "socket_util.h"
#include <stdio.h>
#include <stdlib.h>    // atoi, exit, malloc, free
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h> // sockaddr_in
#include <errno.h>     // errno
#include <string.h>    // strerror
#include <unistd.h>    // STDOUT_FILENO, write, read

const int broadcast_addr_ptrs_size = 10;
struct sockaddr_in* broadcast_addr_ptrs[broadcast_addr_ptrs_size] = {0};
int broadcast_addr_ptrs_empty_idx = 0;
pthread_mutex_t broadcast_addr_ptrs_mutex;

void my_broadcast_addr_ptrs_add(struct sockaddr_in* remote_addr_ptr) {

    pthread_mutex_lock(&broadcast_addr_ptrs_mutex);

    if (broadcast_addr_ptrs_empty_idx >= broadcast_addr_ptrs_size) return;

    int i;
    for (i = 0; i < broadcast_addr_ptrs_size; i++) {
        // do noting if this addr is existent
        if (broadcast_addr_ptrs[i] != NULL &&
            !memcmp(broadcast_addr_ptrs[i], remote_addr_ptr, sizeof *remote_addr_ptr)) {
            return;
        }
    }

    broadcast_addr_ptrs[broadcast_addr_ptrs_empty_idx] = malloc(sizeof *remote_addr_ptr);
    *broadcast_addr_ptrs[broadcast_addr_ptrs_empty_idx++] = *remote_addr_ptr;
    puts("A new peer is added to list!");

    pthread_mutex_unlock(&broadcast_addr_ptrs_mutex);
}

void my_broadcast_except(int bind_socket_desc, char* data_ptr, int data_size, struct sockaddr_in* excepted_addr_ptr) {
    int i;
    for (i = 0; i < broadcast_addr_ptrs_size; i++) {
        if (broadcast_addr_ptrs[i] == NULL) continue;
        if (excepted_addr_ptr != NULL && !memcmp(broadcast_addr_ptrs[i], excepted_addr_ptr, sizeof *excepted_addr_ptr)) continue;
        socket_util_sendto(bind_socket_desc, broadcast_addr_ptrs[i], data_ptr, data_size);
    }
}

#define HI "\0HI"
#define HI_ACK "\0HI_ACK"

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

        if (buffer[0] == '\0') {
            // If is HI or HI_ACK:
            if (!memcmp(buffer, HI, (sizeof HI)-1)) {
                my_broadcast_addr_ptrs_add(&remote_addr);
                // If is not HI_ACK:
                if (memcmp(buffer, HI_ACK, sizeof HI_ACK)) {
                    socket_util_sendto(bind_socket_desc, &remote_addr, HI_ACK, sizeof HI_ACK);
                }
            }
            continue;
        }

        char remote_addr_str[INET_ADDRSTRLEN];
        int remote_port;
        socket_util_sockaddr_get_addr(&remote_addr, remote_addr_str, sizeof remote_addr_str);
        socket_util_sockaddr_get_port(&remote_addr, &remote_port);
        buffer[read_size] = 0;
        printf("[%s:%d] %s", remote_addr_str, remote_port, buffer);

        my_broadcast_except(bind_socket_desc, buffer, read_size, &remote_addr);

    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {

    // Open a socket for binding the address:
    bind_socket_desc = socket_util_socket_datagram();

    // Bind the socket with the address:
    int bind_port = socket_util_bind_addr_port_retry(bind_socket_desc, "0.0.0.0", 5000, 3);

    // Create thread to receive data in background:
    pthread_t my_receiver_thread;
    if (pthread_create(&my_receiver_thread, NULL, my_receiver, NULL)) {
        fprintf(stderr, "Could not create thread: %s\n", strerror(errno));
        exit(1);
    }

    // Try to discover a peer:
    struct sockaddr_in remote_addr = {0};
    socket_util_sockaddr_set_addr(&remote_addr, "127.0.0.1");
    socket_util_sockaddr_set_port(&remote_addr, (bind_port == 5000) ? 5001 : 5000);
    socket_util_sendto(bind_socket_desc, &remote_addr, HI, sizeof HI);


    // Accept message from user:
    char buffer[1024];
    while (1) {
        char* buffer_ptr = fgets(buffer, sizeof buffer, stdin);
        if (buffer_ptr == NULL) {
            fprintf(stderr, "Could not read stdin: %s\n", strerror(errno));
            exit(1);
        }
        my_broadcast_except(bind_socket_desc, buffer, strlen(buffer), NULL);
    }

    // Close bind socket:
    printf("Closing bind socket: ");
    socket_util_close(bind_socket_desc);

    exit(0);
}
