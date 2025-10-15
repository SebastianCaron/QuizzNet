#pragma once

#include <pthread.h>

#define UDP_PORT 5555
#define BUFFER_SIZE 1024

typedef struct {
    const char *server_name;
    const char *port_tcp;
} udp_thread_args;

pthread_t start_udp(const char *SERVER_NAME , const char *PORT_TCP);