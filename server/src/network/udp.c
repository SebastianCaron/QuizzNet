#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>

#include "network.h"
#include "udp.h"
#include "../errors/error.h"
#include "server.h"

void* udp_thread_task(void *arg){
    udp_thread_args *args = (udp_thread_args *)arg;

    int udp_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    if ((udp_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        free(args);
        return NULL;
    }

    int opt = 1;
    setsockopt(udp_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(UDP_PORT);

    if (bind(udp_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(udp_sock);
        free(args);
        return NULL;
    }

    int flags = fcntl(udp_sock, F_GETFL, 0);
    if (flags == -1 || fcntl(udp_sock, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl");
        close(udp_sock);
        free(args);
        return NULL;
    }

    info_log("[UDP] Discovery thread started on port %d", UDP_PORT);

    server_state *state = get_server_state();
    while (!state->should_stop) {
        ssize_t recv_len = recvfrom(udp_sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&client_addr, &client_len);
        
        if (recv_len < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                usleep(500000);
                continue;
            }
            perror("recvfrom");
            continue;
        }
        debug_log("%ld", recv_len);

        buffer[recv_len] = '\0';
        debug_log("[UDP] RECEIVED : %s", buffer);

        if (strcmp(buffer, "looking for quiznet servers") == 0) {
            char response[BUFFER_SIZE];
            snprintf(response, sizeof(response), "hello i'm a quiznet server:%s:%s", args->server_name, args->port_tcp);

            ssize_t sent = sendto(udp_sock, response, strlen(response), 0, (struct sockaddr *)&client_addr, client_len);
            if (sent < 0) {
                perror("sendto");
            } else {
                info_log("[UDP] Responded to %s:%d", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            }
        }
    }

    close(udp_sock);
    free(args);
    return NULL;
}

pthread_t start_udp(const char *SERVER_NAME , const char *PORT_TCP) {
    pthread_t udp_thread;
    udp_thread_args *udp_args = malloc(sizeof(udp_thread_args));
    if (!udp_args) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    
    udp_args->server_name = SERVER_NAME;
    udp_args->port_tcp = PORT_TCP;

    // udp_thread_task(&udp_args);

    if (pthread_create(&udp_thread, NULL, udp_thread_task, udp_args) != 0) {
        perror("pthread_create");
        free(udp_args);
        exit(EXIT_FAILURE);
    }

    server_state *state = get_server_state();
    state->udp_thread = udp_thread;

    info_log("Server started. UDP discovery running in background.");

    return udp_thread;
}