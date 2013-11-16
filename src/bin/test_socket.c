#include <sys/socket.h>
#include <errno.h>     // errno
#include <string.h>    // strerror
#include <stdio.h>
#include <stdlib.h>    // atoi
#include <arpa/inet.h> // sockaddr_in, inet_addr, htons
#include <unistd.h>    // read, write

int main(int argc, char* argv[]) {

    // Create socket:
    int socket_desc = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        fprintf(stderr, "Could not create socket: %s.\n", strerror(errno));
        return 1;
    }
    printf("The socket descriptor is %d.\n", socket_desc);

    // Set server address:
    char* addrstr = "127.0.0.1";
    if (argc >= 2) addrstr = argv[1];
    int port = 80;
    if (argc >= 3) port = atoi(argv[2]);
    struct sockaddr_in server_sin = {0};
    server_sin.sin_addr.s_addr = inet_addr(addrstr);
    server_sin.sin_port = htons(port);

    // Connect to server:
    if (connect(socket_desc, (struct sockaddr*) &server_sin, sizeof server_sin) == -1) {
        fprintf(stderr, "Could not make connection to %s port %d: %s.\n", addrstr, port, strerror(errno));
        return 1;
    }
    printf("Connected to %s:%d.\n", addrstr, port);

    // Send data to server:
    char data[] = "GET / HTTP/1.1\r\n\r\n";
    if (write(socket_desc, data, sizeof data) == -1) {
        fprintf(stderr, "Could not send data: %s.\n", strerror(errno));
        return 1;
    }
    puts("Data sent.");

    // Receive data from server:
    puts("--- Data received ---");
    int read_size;
    char buffer[1024];
    while ((read_size = read(socket_desc, buffer, sizeof buffer)) > 0) {
        write(STDOUT_FILENO, buffer, read_size);
    }
    puts("\n--- End ---");

    // Shutdown socket:
    if (shutdown(socket_desc, SHUT_RDWR) == -1) {
        fprintf(stderr, "Could not shut socket down: %s.\n", strerror(errno));
        return 1;
    }
    puts("The socket is shut down.");

    return 0;
}
