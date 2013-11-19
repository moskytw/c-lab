#include <arpa/inet.h> // sockaddr_in

int socket_util_socket_stream();
int socket_util_socket_datagram();
void socket_util_sockaddr_set_addr(struct sockaddr_in* addr_ptr, char* addr_str_ptr);
void socket_util_sockaddr_set_port(struct sockaddr_in* addr_ptr, int port);
int socket_util_bind_addr_port_retry(int bind_socket_desc, char* bind_addr_str_ptr, int bind_port, int retry_limit);
void socket_util_listen(int bind_socket_desc);
void socket_util_sockaddr_get_addr(struct sockaddr_in* addr_ptr, char* addr_str_ptr, int addr_str_size);
void socket_util_sockaddr_get_port(struct sockaddr_in* addr_ptr, int* port_ptr);
int socket_util_accept_addr_port(int bind_socket_desc, char* remote_addr_str_ptr, int remote_addr_str_size, int* remote_port_ptr);
void socket_util_connect_addr_port(int socket_desc, char* remote_addr_str_ptr, int remote_port);
void socket_util_shutdown_write(int socket_desc);
void socket_util_send(int socket_desc, char* data_ptr, int data_size);
void socket_util_sendto(int socket_desc, struct sockaddr_in* remote_addr_ptr, char* data_ptr, int data_size);
void socket_util_receive(int socket_desc);
void socket_util_close(int file_desc);
