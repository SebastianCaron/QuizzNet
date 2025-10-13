#pragma once

typedef struct {
    char *ip;
    int port;

    int server_fd_udp;
    int server_fd_tcp;

    struct sockaddr_in *address;
} server;


server *start_server(int port);

void destroy_server(server *);