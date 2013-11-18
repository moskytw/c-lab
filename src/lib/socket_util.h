#include <arpa/inet.h> // sockaddr_in

int sockutil_socket_stream();
void sockutil_sockaddr_set_addr(struct sockaddr_in* addr_ptr, char* addr_str);
void sockutil_sockaddr_set_port(struct sockaddr_in* addr_ptr, int port);
void sockutil_bind_addr_port_retry(int bound_socket_desc, char* addr_str, int port, int retry_limit);
void sockutil_listen(int bound_socket_desc);
void sockutil_sockaddr_get_addr(struct sockaddr_in* addr_ptr, char* addr_str, int addr_str_size);
void sockutil_sockaddr_get_port(struct sockaddr_in* addr_ptr, int* port_ptr);
int sockutil_accept_addr_port(int bound_socket_desc, char* addr_str, int addr_str_size, int* port_ptr);
void sockutil_connect_addr_port(int socket_desc, char* addr_str, int port);
void sockutil_shutdown_write(int socket_desc);
void sockutil_send(int socket_desc, char* data, int data_size);
void sockutil_receive(int socket_desc);
int sockutil_close(int file_desc);
