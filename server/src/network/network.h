#pragma once

#define MAX_CLIENTS 512
#define BUFFER_SIZE 1024

typedef struct {
    char *ip;
    int port;

    int server_fd_tcp;

    struct sockaddr_in address;

    int clients[MAX_CLIENTS];
    int number_client;

    char *buffer;
    int size_buffer;
    ssize_t current_size;
} server;


server *start_server(int port);

void destroy_server(server *);