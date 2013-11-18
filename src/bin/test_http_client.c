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

    // Set server address:
    char* addr_str = "127.0.0.1";
    int port = 5000;
    struct sockaddr_in server_addr = {0};
    if (argc >= 2) port = atoi(argv[1]);
    if (argc >= 3) addr_str = argv[2];
    server_addr.sin_family = PF_INET;
    inet_pton(PF_INET, addr_str, &(server_addr.sin_addr));
    server_addr.sin_port = htons(port);

    // Connect to server:
    if (connect(socket_desc, (struct sockaddr*) &server_addr, sizeof server_addr) == -1) {
        fprintf(stderr, "Could not connect to %s on port %d: %s.\n", addr_str, port, strerror(errno));
        my_close(socket_desc);
        exit(1);
    }
    printf("Connected to %s:%d.\n", addr_str, port);

    // Send data to server:
    char data[] = "GET / HTTP/1.1\r\n\r\n";
    if (write(socket_desc, data, sizeof data) == -1) {
        fprintf(stderr, "Could not send data: %s.\n", strerror(errno));
        my_close(socket_desc);
        exit(1);
    }
    puts("Sent data.");

    // Receive data from server:
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

    // Close socket:
    my_close(socket_desc);

    exit(0);
}
