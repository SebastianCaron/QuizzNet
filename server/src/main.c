/**
 * @file main.c
 * @brief QuizzNet server entry point.
 * 
 * This file contains the main function that initializes and runs
 * the QuizzNet game server, including TCP/UDP networking, database
 * initialization, and signal handling for graceful shutdown.
 * 
 * Usage: ./server <SERVER_NAME> <PORT_TCP> [-log]
 * 
 * @authors
 * Sebastian Caron
 * Lina Lethoor
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

#include "network/udp.h"
#include "network/network.h"
#include "db/db.h"
#include "db/initdb.h"
#include "errors/error.h"

#define UDP_PORT 5555

/* Global server pointer for signal handler access */
static server *g_server = NULL;

/**
 * @brief Cleans up all server resources on shutdown.
 * 
 * Performs orderly shutdown:
 * 1. Closes the TCP listening socket
 * 2. Disconnects and destroys all clients
 * 3. Waits for UDP thread to finish
 * 4. Waits for game sessions to complete
 * 5. Closes database and frees server memory
 */
void cleanup_all_resources(void) {
    server_state *state = get_server_state();
    
    if (!state || !state->srv) {
        return;
    }
    
    info_log("Arrêt du serveur en cours...");
    
    /* Close TCP listening socket */
    if (state->srv->server_fd_tcp >= 0) {
        close(state->srv->server_fd_tcp);
        state->srv->server_fd_tcp = -1;
    }
    
    /* Disconnect all clients */
    if (state->srv->clients) {
        int client_count = clist_size(state->srv->clients);
        for (int i = client_count - 1; i >= 0; i--) {
            client *cl = clist_pop(state->srv->clients, i);
            if (cl) {
                client_destroy(cl);
            }
        }
    }
    
    /* Wait for UDP discovery thread */
    pthread_join(state->udp_thread, NULL);
    
    /* Wait for active sessions to finish */
    if (state->srv->sessions) {
        sleep(1);
    }

    /* Cleanup database and server */
    close_db(state->srv);
    destroy_server(state->srv);
    info_log("Serveur arrêté.");
}

/**
 * @brief Signal handler for graceful shutdown.
 * 
 * Catches SIGINT (Ctrl+C) and sets the should_stop flag
 * to trigger graceful shutdown of all server components.
 * 
 * @param sig Signal number received.
 */
void signal_handler(int sig) {
    if (sig == SIGINT) {
        server_state *state = get_server_state();
        state->should_stop = 1;
    }
}

/**
 * @brief Main entry point for the QuizzNet server.
 * 
 * Initializes all server components and runs the main loop:
 * 1. Parses command line arguments
 * 2. Optionally enables debug logging
 * 3. Starts UDP discovery service
 * 4. Starts TCP server
 * 5. Initializes database
 * 6. Runs main client processing loop
 * 7. Cleans up on shutdown
 * 
 * @param argc Argument count.
 * @param argv Arguments: <SERVER_NAME> <PORT_TCP> [-log]
 * @return EXIT_SUCCESS on clean shutdown, EXIT_FAILURE on error.
 */
int main(int argc, char *argv[]) {
    /* Validate arguments */
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <SERVER_NAME> <PORT_TCP>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *SERVER_NAME = argv[1];
    const char *PORT_TCP = argv[2];

    /* Enable debug logging if -log flag is present */
    char log_debug = argc >= 4 && !strcmp(argv[3], "-log");
    if(log_debug){
        init_debug_log();
    }

    /* Setup signal handler for graceful shutdown */
    signal(SIGINT, signal_handler);

    /* Start UDP discovery service */
    start_udp(SERVER_NAME, PORT_TCP);

    /* Start TCP server */
    g_server = start_server(atoi(argv[2]));
    if (!g_server) {
        fprintf(stderr, "Erreur lors du démarrage du serveur\n");
        exit(EXIT_FAILURE);
    }
    
    /* Initialize global server state */
    init_server_state(g_server);

    /* Initialize database */
    add_sqlite_to_server(g_server);
    init_db(g_server);
    
    /* Main server loop */
    server_state *state = get_server_state();
    while(!state->should_stop){
        server_client_procedure(g_server);
        usleep(100000);  /* 100ms delay between iterations */
    }

    /* Cleanup and exit */
    cleanup_all_resources();
    close_debug_log();
    
    return EXIT_SUCCESS;
}
