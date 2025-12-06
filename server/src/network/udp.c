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

/**
 * @brief Main function executed by the UDP discovery thread.
 * 
 * This function runs in a separate thread and handles UDP discovery requests.
 * It listens on UDP_PORT for broadcast messages from clients looking for
 * QuizzNet servers and responds with server information.
 * 
 * Protocol:
 * - Client sends: "looking for quiznet servers"
 * - Server responds: "hello i'm a quiznet server:<server_name>:<tcp_port>"
 * 
 * @param arg Pointer to udp_thread_args containing server name and TCP port.
 * @return NULL on thread exit.
 */
void* udp_thread_task(void *arg){
    udp_thread_args *args = (udp_thread_args *)arg;

    int udp_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    /* Create UDP socket (SOCK_DGRAM for connectionless datagrams) */
    if ((udp_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        free(args);
        return NULL;
    }

    /* Allow socket address reuse to avoid "Address already in use" errors */
    int opt = 1;
    setsockopt(udp_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    /* Configure server address */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(UDP_PORT);

    /* Bind socket to the configured address and port */
    if (bind(udp_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(udp_sock);
        free(args);
        return NULL;
    }

    /* Set socket to non-blocking mode to allow periodic shutdown checks */
    int flags = fcntl(udp_sock, F_GETFL, 0);
    if (flags == -1 || fcntl(udp_sock, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl");
        close(udp_sock);
        free(args);
        return NULL;
    }

    info_log("[UDP] Discovery thread started on port %d", UDP_PORT);

    /* Main loop: listen for discovery requests until server shutdown */
    server_state *state = get_server_state();
    while (!state->should_stop) {
        ssize_t recv_len = recvfrom(udp_sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&client_addr, &client_len);
        
        if (recv_len < 0) {
            /* Non-blocking socket: no data available, sleep and retry */
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                usleep(500000);  /* Sleep 500ms before next check */
                continue;
            }
            perror("recvfrom");
            continue;
        }
        debug_log("%ld", recv_len);

        /* Null-terminate the received message */
        buffer[recv_len] = '\0';
        debug_log("[UDP] RECEIVED : %s", buffer);

        /* Check if this is a valid discovery request */
        if (strcmp(buffer, "looking for quiznet servers") == 0) {
            /* Build response with server name and TCP port */
            char response[BUFFER_SIZE];
            snprintf(response, sizeof(response), "hello i'm a quiznet server:%s:%s", args->server_name, args->port_tcp);

            /* Send response back to the requesting client */
            ssize_t sent = sendto(udp_sock, response, strlen(response), 0, (struct sockaddr *)&client_addr, client_len);
            if (sent < 0) {
                perror("sendto");
            } else {
                info_log("[UDP] Responded to %s:%d", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            }
        }
    }

    /* Cleanup: close socket and free arguments */
    close(udp_sock);
    free(args);
    return NULL;
}

pthread_t start_udp(const char *SERVER_NAME, const char *PORT_TCP) {
    pthread_t udp_thread;
    
    /* Allocate arguments struct (will be freed by the thread) */
    udp_thread_args *udp_args = malloc(sizeof(udp_thread_args));
    if (!udp_args) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    
    udp_args->server_name = SERVER_NAME;
    udp_args->port_tcp = PORT_TCP;

    /* Create the UDP discovery thread */
    if (pthread_create(&udp_thread, NULL, udp_thread_task, udp_args) != 0) {
        perror("pthread_create");
        free(udp_args);
        exit(EXIT_FAILURE);
    }

    /* Store thread handle in global server state for later cleanup */
    server_state *state = get_server_state();
    state->udp_thread = udp_thread;

    info_log("Server started. UDP discovery running in background.");

    return udp_thread;
}
