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
#include "../errors/error.h"

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

    res->buffer = calloc(BUFFER_SIZE, sizeof(char));
    if(res->buffer == NULL){
        throw_error(MEMORY_ALLOCATION, NULL);
        return NULL;
    }

    res->current_size = 0;
    res->size_buffer = BUFFER_SIZE;

    res->server_fd_tcp = socket(AF_INET, SOCK_STREAM, 0);
    if (res->server_fd_tcp < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(res->server_fd_tcp, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    (res->address).sin_family = AF_INET;
    (res->address).sin_addr.s_addr = INADDR_ANY;
    (res->address).sin_port = htons(port);

    if (bind(res->server_fd_tcp, (struct sockaddr *)&(res->address), sizeof(res->address)) < 0) {
        perror("bind");
        close(res->server_fd_tcp);
        exit(EXIT_FAILURE);
    }

    if (listen(res->server_fd_tcp, MAX_CLIENTS) < 0) {
        perror("listen");
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

        if (s->number_client == MAX_CLIENTS) {
            printf("[TCP] Too many clients, refusing %s\n", inet_ntoa(client_addr.sin_addr));
            close(client_fd);
        } else {
            s->clients[s->number_client] = client_fd;
            s->number_client++;
            printf("[TCP] Client connected: %s:%d (fd=%d)\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), client_fd);
        }
    }
}

void destroy_server(server *s){

}

void receive_from(server *s, int i){
    int fd = s->clients[i];
    if (fd == -1) return;

    s->current_size = 0;
    ssize_t bytes;
    bytes = recv(fd, s->buffer, sizeof(char) * s->size_buffer - 1, 0);
    s->current_size += bytes;
    while(bytes > 0){
        // REALLOCATE BUFFER
        if(s->current_size == s->size_buffer-1){
            void *old_buffer = s->buffer;
            s->buffer = realloc(s->buffer, (s->size_buffer + BUFFER_SIZE) * sizeof(char));
            if(s->buffer == NULL){
                s->buffer = old_buffer;
                throw_error(MEMORY_ALLOCATION, NULL);
                return;
            }
            s->current_size += BUFFER_SIZE;
        }


        bytes = recv(fd, s->buffer + s->current_size, s->size_buffer * sizeof(char) - 1, 0);
        s->current_size += bytes;
    }
    
    if (bytes < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
        perror("recv");
        close(fd);
        s->clients[i] = s->clients[s->number_client-1]; // C'est moche
        s->number_client--;
    }
}

void server_client_procedure(server *s){
    
}