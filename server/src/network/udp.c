#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>

#include "network.h"
#include "udp.h"
#include "../errors/error.h"

void udp_thread(void *arg){
    udp_thread_args *args = (udp_thread_args *)arg;

    int udp_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    if ((udp_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        pthread_exit(NULL);
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
        pthread_exit(NULL);
    }

    printf("[UDP] Discovery thread started on port %d\n", UDP_PORT);

    while (1) {
        ssize_t recv_len = recvfrom(udp_sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&client_addr, &client_len);
        if (recv_len < 0) {
            perror("recvfrom");
            continue;
        }

        buffer[recv_len] = '\0';

        if (strcmp(buffer, "looking for quiznet servers") == 0) {
            char response[BUFFER_SIZE];
            snprintf(response, sizeof(response), "hello i’m a quiznet server:%s:%s", args->server_name, args->port_tcp);

            ssize_t sent = sendto(udp_sock, response, strlen(response), 0, (struct sockaddr *)&client_addr, client_len);
            if (sent < 0) {
                perror("sendto");
            } else {
                printf("[UDP] Responded to %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            }
        }
    }

    close(udp_sock);
    pthread_exit(NULL);
}

pthread_t start_udp(const char *SERVER_NAME , const char *PORT_TCP) {
    pthread_t udp_thread;
    udp_thread_args udp_args = {SERVER_NAME, PORT_TCP};

    if (pthread_create(&udp_thread, NULL, udp_thread, &udp_args) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    printf("[MAIN] Server started. UDP discovery running in background.\n");

    return udp_thread;
}