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

// Variable globale pour l'état du serveur
static server_state g_server_state = {0};

void init_server_state(server *s) {
    g_server_state.should_stop = 0;
    g_server_state.srv = s;
}

void cleanup_server_state(void) {
    g_server_state.should_stop = 1;
}

server_state* get_server_state(void) {
    return &g_server_state;
}

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

    info_log("[TCP] Listening on port %d", port);

    return res;
}

void accept_new_connection(server *s){
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    int client_fd = accept(s->server_fd_tcp, (struct sockaddr *)&client_addr, &len);

    if (client_fd >= 0) {
        set_nonblocking(client_fd);

        if (clist_size(s->clients) == MAX_CLIENTS) {
            info_log("[TCP] Too many clients, refusing %s", inet_ntoa(client_addr.sin_addr));
            close(client_fd);
        } else {
            client *nc = client_init(client_fd);
            if(!nc){
                throw_error(MEMORY_ALLOCATION, NULL);
                return;
            }
            clist_append(s->clients, nc);
            info_log("[TCP] Client connected: %s:%d (fd=%d)", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), client_fd);
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

int receive_from(int fd, char **buffer, ssize_t *size, int *capacity){
    if (fd < 0) return 0;

    ssize_t bytes = 0;
    (*size) = 0;

    while (1) {
        size_t remaining = (*capacity) - (*size);

        if (remaining == 0) {
            size_t new_size = (*capacity) + BUFFER_SIZE;
            void *tmp = realloc((*buffer), new_size);
            if (!tmp) {
                throw_error(MEMORY_ALLOCATION, "Erreur reallocation dans receive_from");
                return -2;
            }
            (*buffer) = tmp;
            (*capacity) = new_size;
            remaining = BUFFER_SIZE;
        }

        bytes = recv(fd, (*buffer) + (*size), remaining, 0);

        if (bytes > 0) {
            (*size) += bytes;
            continue;
        }

        if (bytes == 0) {
            return -1;
        }

        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            break;
        }

        perror("recv");
        return -2;
    }

    if ((*size) < (*capacity))
        (*buffer)[(*size)] = '\0';

    // info_log("[TCP] Reçu %d octets du client fd=%d", (*size), fd);
    return (*size);
}

int server_receive_from(server *s, int i) {
    client *client = clist_get(s->clients, i);

    int fd = client->fd;
    if (fd < 0) return 0;

    int res = receive_from(fd, &(s->buffer), &(s->current_size), &(s->size_buffer));

    if(res == -1){
        info_log("[TCP] Client %d closed connection.", i);
    }

    return res;
}


void server_client_procedure(server *s){
    server_state *state = get_server_state();
    if (state->should_stop) {
        return;
    }
    
    accept_new_connection(s);
    int resp = 0;
    int i = 0;
    while(i < clist_size(s->clients)){
        if (state->should_stop) {
            break;
        }
        resp = server_receive_from(s, i);
        if(resp == -1){
            client *cl = clist_pop(s->clients, i);
            client_destroy(cl);
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