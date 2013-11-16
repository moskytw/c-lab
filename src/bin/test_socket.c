#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h> // sockaddr_in, inet_addr, htons
#include <unistd.h> // read, write, close

int main(int argc, char* argv[]) {

    int socket_desc;
    socket_desc = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        fprintf(stderr, "Could not create socket.\n");
        return 1;
    }
    printf("A socket is created. The socket descriptor is %d.\n", socket_desc);

    char addrstr[] = "127.0.0.1";
    int port = 5000;
    struct sockaddr_in server_sin = {0};
    server_sin.sin_addr.s_addr = inet_addr(addrstr);
    server_sin.sin_port = htons(port);

    if (connect(socket_desc, (struct sockaddr*) &server_sin, sizeof(server_sin)) == -1) {
        fprintf(stderr, "Could not make connection to %s:%d\n", addrstr, port);
        return 1;
    }
    printf("Connected to %s:%d.\n", addrstr, port);

    if (close(socket_desc) == -1) {
        fprintf(stderr, "Could not close socket.\n");
    }
    puts("The socket is closed.");

    return 0;
}
