#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>

#include <fcntl.h>
#include <netinet/in.h>

#include "network.h"
#include "client.h"
#include "../errors/error.h"
#include "../endpoints/endpoints.h"
#include "../utils/chained_list.h"

int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

server *start_server(int port){
    server *res = calloc(1, sizeof(server));

    if(res == NULL){
        throw_error(MEMORY_ALLOCATION, NULL);
        return NULL;
    }

    res->clients = clist_init();
    if(res->clients == NULL){
        throw_error(MEMORY_ALLOCATION, "Erreur allocation liste chainee dans start server (clients)");
        return NULL;
    }

    res->sessions = clist_init();
    if(res->sessions == NULL){
        throw_error(MEMORY_ALLOCATION, "Erreur allocation liste chainee dans start server (sessions)");
        return NULL;
    }

    res->session_counter = 0;

    res->buffer = calloc(BUFFER_SIZE, sizeof(char));
    if(res->buffer == NULL){
        throw_error(MEMORY_ALLOCATION, NULL);
        return NULL;
    }

    res->current_size = 0;
    res->size_buffer = BUFFER_SIZE;

    res->server_fd_tcp = socket(AF_INET, SOCK_STREAM, 0);
    if (res->server_fd_tcp < 0) {
        throw_error(SOCKET, NULL);
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(res->server_fd_tcp, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    (res->address).sin_family = AF_INET;
    (res->address).sin_addr.s_addr = INADDR_ANY;
    (res->address).sin_port = htons(port);

    if (bind(res->server_fd_tcp, (struct sockaddr *)&(res->address), sizeof(res->address)) < 0) {
        throw_error(BIND, NULL);
        close(res->server_fd_tcp);
        exit(EXIT_FAILURE);
    }

    if (listen(res->server_fd_tcp, MAX_CLIENTS) < 0) {
        throw_error(LISTEN, NULL);
        close(res->server_fd_tcp);
        exit(EXIT_FAILURE);
    }

    set_nonblocking(res->server_fd_tcp);

    printf("[TCP] Listening on port %d\n", port);

    return res;
}

void accept_new_connection(server *s){
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    int client_fd = accept(s->server_fd_tcp, (struct sockaddr *)&client_addr, &len);

    if (client_fd >= 0) {
        set_nonblocking(client_fd);

        if (clist_size(s->clients) == MAX_CLIENTS) {
            printf("[TCP] Too many clients, refusing %s\n", inet_ntoa(client_addr.sin_addr));
            close(client_fd);
        } else {
            client *nc = client_init(client_fd);
            if(!nc){
                throw_error(MEMORY_ALLOCATION, NULL);
                return;
            }
            clist_append(s->clients, nc);
            printf("[TCP] Client connected: %s:%d (fd=%d)\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), client_fd);
        }
    }
}

void destroy_server(server *s){
    if(!s) return;
    clist_destroy(s->clients);
    clist_destroy(s->sessions);
    free(s->buffer);
    free(s);
}

int receive_from(server *s, int i) {
    client *client = clist_get(s->clients, i);

    int fd = client->fd;
    if (fd < 0) return 0;

    ssize_t bytes = 0;
    s->current_size = 0;

    while (1) {
        size_t remaining = s->size_buffer - s->current_size;

        if (remaining == 0) {
            size_t new_size = s->size_buffer + BUFFER_SIZE;
            void *tmp = realloc(s->buffer, new_size);
            if (!tmp) {
                throw_error(MEMORY_ALLOCATION, "Erreur reallocation dans receive_from");
                return -2;
            }
            s->buffer = tmp;
            s->size_buffer = new_size;
            remaining = BUFFER_SIZE;
        }

        bytes = recv(fd, s->buffer + s->current_size, remaining, 0);

        if (bytes > 0) {
            s->current_size += bytes;
            continue;
        }

        if (bytes == 0) {
            printf("[TCP] Client %d closed connection.\n", i);
            return -1;
        }

        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            break;
        }

        perror("recv");
        return -2;
    }

    if (s->current_size < s->size_buffer)
        s->buffer[s->current_size] = '\0';

    // printf("[TCP] Reçu %zd octets du client fd=%d\n", s->current_size, fd);
    return s->current_size;
}


void server_client_procedure(server *s){
    accept_new_connection(s);
    int resp = 0;
    int i = 0;
    while(i < clist_size(s->clients)){
        resp = receive_from(s, i);
        if(resp == -1){
            clist_pop(s->clients, i);
            continue;
        }
        if(resp > 0){
            handle_request(s, s->buffer, (client *)clist_get(s->clients, i));
        }
        i++;
    }
}

void remove_client_from_server_procedure(server *s, client *cl){
    if(!clist_remove(s->clients, cl)){
        throw_error(NOT_FOUND, "Error, client not attached to server.");
    }
}

void attach_client_to_server_procedure(server *s, client *cl){
    if(!s) return;
    if(!cl) return;
    if(clist_append(s->clients, cl)){
        throw_error(CLIENT_ATTACH, "Error, fail to attach client to server procedure");
        client_destroy(cl);
    }
}