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
#include "../utils/buffer_requests.h"
#include "../game_logic/session.h"

/* Global server state instance */
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

/**
 * @brief Sets a file descriptor to non-blocking mode.
 * 
 * @param fd File descriptor to modify.
 * @return 0 on success, -1 on failure.
 */
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

    /* Initialize client list */
    res->clients = clist_init();
    if(res->clients == NULL){
        throw_error(MEMORY_ALLOCATION, "Erreur allocation liste chainee dans start server (clients)");
        return NULL;
    }

    /* Initialize session list */
    res->sessions = clist_init();
    if(res->sessions == NULL){
        throw_error(MEMORY_ALLOCATION, "Erreur allocation liste chainee dans start server (sessions)");
        return NULL;
    }

    res->session_counter = 0;

    /* Allocate receive buffer */
    res->buffer = calloc(BUFFER_SIZE, sizeof(char));
    if(res->buffer == NULL){
        throw_error(MEMORY_ALLOCATION, NULL);
        return NULL;
    }

    res->current_size = 0;
    res->size_buffer = BUFFER_SIZE;

    /* Create TCP socket */
    res->server_fd_tcp = socket(AF_INET, SOCK_STREAM, 0);
    if (res->server_fd_tcp < 0) {
        throw_error(SOCKET, NULL);
        exit(EXIT_FAILURE);
    }

    /* Allow socket address reuse */
    int opt = 1;
    setsockopt(res->server_fd_tcp, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    /* Configure server address */
    (res->address).sin_family = AF_INET;
    (res->address).sin_addr.s_addr = INADDR_ANY;
    (res->address).sin_port = htons(port);

    /* Bind socket to address */
    if (bind(res->server_fd_tcp, (struct sockaddr *)&(res->address), sizeof(res->address)) < 0) {
        throw_error(BIND, NULL);
        close(res->server_fd_tcp);
        exit(EXIT_FAILURE);
    }

    /* Start listening for connections */
    if (listen(res->server_fd_tcp, MAX_CLIENTS) < 0) {
        throw_error(LISTEN, NULL);
        close(res->server_fd_tcp);
        exit(EXIT_FAILURE);
    }

    /* Set non-blocking mode for accept() to not block the main loop */
    set_nonblocking(res->server_fd_tcp);

    info_log("[TCP] Listening on port %d", port);

    return res;
}

/**
 * @brief Accepts a new client connection if one is pending.
 * 
 * Called by the main loop to check for and accept new connections.
 * The new client is added to the server's client list.
 * 
 * @param s Pointer to the server structure.
 */
void accept_new_connection(server *s){
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    int client_fd = accept(s->server_fd_tcp, (struct sockaddr *)&client_addr, &len);

    if (client_fd >= 0) {
        /* Set client socket to non-blocking */
        set_nonblocking(client_fd);

        /* Check if we've reached the maximum number of clients */
        if (clist_size(s->clients) == MAX_CLIENTS) {
            info_log("[TCP] Too many clients, refusing %s", inet_ntoa(client_addr.sin_addr));
            close(client_fd);
        } else {
            /* Create and register new client */
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

int receive_from(int fd, buffer *b){
    if (fd < 0 || !b) return 0;

    /* Initialize capacity if not set */
    if (b->capacity == 0) {
        b->capacity = MAX_BUFFER_SIZE;
    }

    /* Receive data and append to buffer */
    ssize_t bytes = recv(fd, b->buffer + b->size, b->capacity - b->size - 1, 0);
    
    if (bytes < 0) {
        /* Receive error */
        return -2;
    }
    
    if (bytes == 0) {
        /* Connection closed by peer */
        return -1;
    }
    
    /* Update buffer size and null-terminate */
    b->size += bytes;
    b->buffer[b->size] = '\0';
    
    return b->size;
}

/**
 * @brief Receives data from a specific client by index.
 * 
 * Reads available data from the client's socket into their buffer.
 * 
 * @param s Pointer to the server.
 * @param i Index of the client in the clients list.
 * @return Number of bytes in buffer, -1 if connection closed, 0 on error or no data.
 */
int server_receive_from(server *s, int i) {
    client *client = clist_get(s->clients, i);
    if (!client) return 0;

    int fd = client->fd;
    if (fd < 0) return 0;

    int res = receive_from(fd, &client->buffer_cl);

    if(res == -1){
        info_log("[TCP] Client %d closed connection.", i);
    } else if(res == -2){
        info_log("[TCP] Client %d error.", i);
        return 0;
    }

    return res;
}


void server_client_procedure(server *s){
    server_state *state = get_server_state();
    if (state->should_stop) {
        return;
    }
    
    /* Check for new connections */
    accept_new_connection(s);

    /* Process each connected client */
    int resp = 0;
    int i = 0;
    while(i < clist_size(s->clients)){
        /* Check for shutdown signal */
        if (state->should_stop) {
            break;
        }

        resp = server_receive_from(s, i);

        /* Client disconnected, remove from list */
        if(resp == -1){
            client *cl = clist_pop(s->clients, i);
            if(cl && cl->infos_session.session){
                session_remove_client(cl->infos_session.session, cl);
            }
            client_destroy(cl);
            continue;
        }
        client *cl = (client *)clist_get(s->clients, i);
        if(resp > 0 && request_available(&cl->buffer_cl)){
            char *request = get_request(&cl->buffer_cl);
            if(request){
                handle_request(s, request, cl);
                update_buffer(&cl->buffer_cl, strlen(request));
                free(request);
            }
        }
        reset_full_buffer(&cl->buffer_cl);
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
