#pragma once

#define MAX_CLIENTS 512
#define BUFFER_SIZE 1024

#include <arpa/inet.h>

#include "../utils/chained_list.h"

typedef struct {
    char *ip;
    int port;

    int server_fd_tcp;

    struct sockaddr_in address;

    chained_list *clients;

    char *buffer;
    int size_buffer;
    ssize_t current_size;
} server;




server *start_server(int port);

void destroy_server(server *);

void server_client_procedure(server *s);