#include <stdio.h>
#include <stdlib.h>    // atoi, exit, malloc, free
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h> // sockaddr_in
#include <errno.h>     // errno
#include <string.h>    // strerror
#include <unistd.h>    // STDOUT_FILENO, write, read

void my_exit_if_errno(char* message_tmpl) {
    if (errno == 0) return;
    fprintf(stderr, message_tmpl, strerror(errno));
    exit(1);
}

void my_sockaddr_set(struct sockaddr* addr_ptr, char* addr_str_ptr, int port) {

    struct sockaddr_in* addr_in_ptr = (struct sockaddr_in*) addr_ptr;

    if (addr_str_ptr != NULL) {
        addr_in_ptr->sin_family = PF_INET;
        inet_pton(PF_INET, addr_str_ptr, &(addr_in_ptr->sin_addr));
    }

    if (port != -1) {
        addr_in_ptr->sin_port = htons(port);
    }
}

void my_sockaddr_get(struct sockaddr* addr_ptr, char* addr_str_ptr, int addr_str_size, int* port_ptr) {

    struct sockaddr_in* addr_in_ptr = (struct sockaddr_in*) addr_ptr;

    if (addr_str_ptr != NULL) {
        inet_ntop(PF_INET, &(addr_in_ptr->sin_addr), addr_str_ptr, addr_str_size);
    }

    if (port_ptr != NULL) {
        *port_ptr = ntohs(addr_in_ptr->sin_port);
    }
}

const int broadcast_addr_ptrs_size = 10;
struct sockaddr* broadcast_addr_ptrs[broadcast_addr_ptrs_size] = {0};
int broadcast_addr_ptrs_empty_idx = 0;
pthread_mutex_t broadcast_addr_ptrs_mutex;

void my_broadcast_addr_ptrs_add(struct sockaddr* remote_addr_ptr) {

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

void my_broadcast_except(int bind_socket_desc, char* data_ptr, int data_size, struct sockaddr* excepted_addr_ptr) {
    int i;
    for (i = 0; i < broadcast_addr_ptrs_size; i++) {
        if (broadcast_addr_ptrs[i] == NULL) continue;
        if (excepted_addr_ptr != NULL && !memcmp(broadcast_addr_ptrs[i], excepted_addr_ptr, sizeof *excepted_addr_ptr)) continue;
        sendto(bind_socket_desc, data_ptr, data_size, 0, broadcast_addr_ptrs[i], sizeof *broadcast_addr_ptrs[i]);
    }
}

#define HI "\0HI"
#define HI_ACK "\0HI_ACK"

int bind_socket_desc;

void* my_receiver() {

    int read_size;
    char buffer[1024];
    struct sockaddr remote_addr = {0};
    int remote_addr_size = sizeof remote_addr;

    while ((read_size = recvfrom(bind_socket_desc, buffer, sizeof buffer, 0, &remote_addr, (socklen_t*) &remote_addr_size))) {

        my_exit_if_errno("Could not read data: %s.\n");

        if (buffer[0] == '\0') {
            // If is HI or HI_ACK:
            if (!memcmp(buffer, HI, (sizeof HI)-1)) {
                my_broadcast_addr_ptrs_add(&remote_addr);
                // If is not HI_ACK:
                if (memcmp(buffer, HI_ACK, sizeof HI_ACK)) {
                    sendto(bind_socket_desc, HI_ACK, sizeof HI_ACK, 0, &remote_addr, sizeof remote_addr);
                }
            }
            continue;
        }

        char remote_addr_str[INET_ADDRSTRLEN];
        int remote_port;
        my_sockaddr_get(&remote_addr, remote_addr_str, sizeof remote_addr_str, &remote_port);
        buffer[read_size] = 0;
        printf("[%s:%d] %s", remote_addr_str, remote_port, buffer);

        my_broadcast_except(bind_socket_desc, buffer, read_size, &remote_addr);

    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {

    // Open a socket for binding the address:
    bind_socket_desc = socket(PF_INET, SOCK_DGRAM, 0);
    my_exit_if_errno("Could not create stream socket: %s.\n");

    // Bind the socket with the address:

    char bind_addr_str[] = "0.0.0.0";
    int bind_port = 5000;
    struct sockaddr bind_addr = {0};
    my_sockaddr_set(&bind_addr, bind_addr_str, bind_port);

    int retry_limit = 3;
    while (retry_limit--) {
        if (bind(bind_socket_desc, &bind_addr, (socklen_t) sizeof bind_addr) == 0) break;
        if (errno == EADDRINUSE) {
            errno = 0;
            printf("The address %s port %d is in use. Try next port.\n",
                bind_addr_str, bind_port
            );
            my_sockaddr_set(&bind_addr, NULL, ++bind_port);
            continue;
        }
        my_exit_if_errno("Could not bind: %s.\n");
    }
    my_exit_if_errno("Could not find a unused port: %s.\n");
    printf("Bound to %s:%d.\n", bind_addr_str, bind_port);

    // Create thread to receive data in background:
    pthread_t my_receiver_thread;
    if (pthread_create(&my_receiver_thread, NULL, my_receiver, NULL)) {
        fprintf(stderr, "Could not create thread: %s.\n", strerror(errno));
        exit(1);
    }

    // Try to discover a peer:
    struct sockaddr remote_addr = {0};
    my_sockaddr_set(&remote_addr, "127.0.0.1", (bind_port == 5000) ? 5001 : 5000);
    sendto(bind_socket_desc, HI, sizeof HI, 0, &remote_addr, sizeof remote_addr);

    // Accept message from user:
    while (1) {
        char buffer[1024];
        fgets(buffer, sizeof buffer, stdin);
        my_broadcast_except(bind_socket_desc, buffer, strlen(buffer), NULL);
    }

    // Close bind socket:
    printf("Closing bind socket: ");
    close(bind_socket_desc);

    exit(0);
}
