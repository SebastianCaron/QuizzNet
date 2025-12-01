#pragma once

#include <arpa/inet.h>
#include <pthread.h>

#include "../utils/chained_list.h"
#include "../sqlite/sqlite3.h"

typedef struct {
    char *ip;
    int port;

    int server_fd_tcp;

    struct sockaddr_in address;

    chained_list *clients;
    chained_list *sessions;
    int session_counter;

    char *buffer;
    int size_buffer;
    ssize_t current_size;

    sqlite3 *db;

} server;

typedef struct {
    int should_stop;
    pthread_t udp_thread;
    server *srv;
} server_state;
