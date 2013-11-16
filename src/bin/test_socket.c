#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h> // sockaddr_in, inet_addr, htons

int main(int argc, char* argv[]) {

    // Create socket:
    int socket_desc;
    socket_desc = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        fprintf(stderr, "Could not create socket.\n");
        return 1;
    }
    printf("A socket is created. The socket descriptor is %d.\n", socket_desc);

    // Set server address:
    char addrstr[] = "127.0.0.1";
    int port = 5000;
    struct sockaddr_in server_sin = {0};
    server_sin.sin_addr.s_addr = inet_addr(addrstr);
    server_sin.sin_port = htons(port);

    // Connect to server:
    if (connect(socket_desc, (struct sockaddr*) &server_sin, sizeof server_sin) == -1) {
        fprintf(stderr, "Could not make connection to %s:%d\n", addrstr, port);
        return 1;
    }
    printf("Connected to %s:%d.\n", addrstr, port);

    // Send data to server:
    char data[] = "GET / HTTP/1.1\r\n\r\n";
    if (send(socket_desc, data, sizeof data, 0) == -1) {
        fprintf(stderr, "Could not send data.\n");
        return 1;
    }
    puts("Data sent.");

    // Receive data from server:
    int read_size = 0;
    char buffer[1024];
    puts("--- Data received ---");
    while ((read_size = recv(socket_desc, buffer, sizeof buffer, 0)) > 0) {
        buffer[read_size] = 0;
        printf("%s", buffer);
    }
    puts("\n--- End ---");

    // Shutdown socket:
    if (shutdown(socket_desc, SHUT_RDWR) == -1) {
        fprintf(stderr, "Could not shut socket down.\n");
        return 1;
    }
    puts("The socket is shut down.");

    return 0;
}
