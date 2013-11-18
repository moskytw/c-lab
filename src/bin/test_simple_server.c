#include <sys/socket.h>
#include <errno.h>     // errno
#include <string.h>    // strerror
#include <stdio.h>
#include <stdlib.h>    // atoi
#include <arpa/inet.h> // sockaddr_in, inet_pton, htons
#include <unistd.h>    // write, read, exit

int my_close(int file_desc) {
    int return_val;
    if ((return_val = close(file_desc)) == -1) {
        fprintf(stderr, "Could not close socket: %s.\n", strerror(errno));
        exit(1);
    }
    puts("The socket is closed.");
    return return_val;
}

int main(int argc, char* argv[]) {

    // Open socket:
    int socket_desc = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        fprintf(stderr, "Could not create socket: %s.\n", strerror(errno));
        exit(1);
    }
    puts("The socket is opened.");

    // Set server (my) address:
    char* addr_str = "0.0.0.0";
    int port = 5000;
    struct sockaddr_in server_addr = {0};
    if (argc >= 2) port = atoi(argv[1]);
    if (argc >= 3) addr_str = argv[2];
    server_addr.sin_family = PF_INET;
    inet_pton(PF_INET, addr_str, &(server_addr.sin_addr));
    server_addr.sin_port = htons(port);

    // Bind socket with this address:
    if (bind(socket_desc, (struct sockaddr*) &server_addr, (socklen_t) sizeof server_addr) == -1) {
        fprintf(stderr, "Could not bind %s on port %d: %s.\n", addr_str, port, strerror(errno));
        my_close(socket_desc);
        exit(1);
    }
    printf("Bound to %s:%d.\n", addr_str, port);

    // Listen for a connection from client:
    if (listen(socket_desc, 0) == -1) {
        fprintf(stderr, "Could not listen: %s.\n", strerror(errno));
        my_close(socket_desc);
        exit(1);
    }
    printf("Listening...\n");

    // Accept for a connection from client:
    struct sockaddr_in client_addr = {0};
    socklen_t client_addr_size = sizeof client_addr;
    int accept_desc;
    if ((accept_desc = accept(socket_desc, (struct sockaddr*) &client_addr, &client_addr_size)) == -1) {
        fprintf(stderr, "Could not accept: %s.\n", strerror(errno));
        my_close(socket_desc);
        exit(1);
    }
    printf("Accepted a connection.\n");

    // Receive data from client:
    int read_size;
    char buffer[1024];
    if ((read_size = read(accept_desc, buffer, sizeof buffer)) == -1) {
        fprintf(stderr, "Could not read data: %s.\n", strerror(errno));
        my_close(accept_desc);
        exit(1);
    }
    puts("--- Data received ---");
    write(STDOUT_FILENO, buffer, read_size);
    puts("--- End ---");

    // Send data to client:
    char data[] = "OK";
    if (write(accept_desc, data, sizeof data) == -1) {
        fprintf(stderr, "Could not send data: %s.\n", strerror(errno));
        my_close(accept_desc);
        exit(1);
    }
    puts("Sent data.");

    // Close socket:
    my_close(socket_desc);

    exit(0);
}
