#pragma once

#define MAX_CLIENTS 512
#define BUFFER_SIZE 1024

#include "server.h"
#include "client.h"
#include "../utils/buffer_requests.h"

int receive_from(int fd, buffer *b);

server *start_server(int port);

void destroy_server(server *);

void server_client_procedure(server *s);

void remove_client_from_server_procedure(server *s, client *cl);

void attach_client_to_server_procedure(server *s, client *cl);