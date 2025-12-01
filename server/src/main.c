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

static server *g_server = NULL;

void cleanup_all_resources(void) {
    server_state *state = get_server_state();
    
    if (!state || !state->srv) {
        return;
    }
    
    info_log("Arrêt du serveur en cours...");
    
    if (state->srv->server_fd_tcp >= 0) {
        close(state->srv->server_fd_tcp);
        state->srv->server_fd_tcp = -1;
    }
    
    if (state->srv->clients) {
        int client_count = clist_size(state->srv->clients);
        for (int i = client_count - 1; i >= 0; i--) {
            client *cl = clist_pop(state->srv->clients, i);
            if (cl) {
                client_destroy(cl);
            }
        }
    }
    
    pthread_join(state->udp_thread, NULL);
    
    if (state->srv->sessions) {
        sleep(1);
    }
    close_db(state->srv);
    destroy_server(state->srv);
    info_log("Serveur arrêté.");
}

void signal_handler(int sig) {
    if (sig == SIGINT) {
        server_state *state = get_server_state();
        state->should_stop = 1;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <SERVER_NAME> <PORT_TCP>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *SERVER_NAME = argv[1];
    const char *PORT_TCP = argv[2];

    char log_debug = argc >= 4 && !strcmp(argv[3], "-log");
    if(log_debug){
        init_debug_log();
    }

    signal(SIGINT, signal_handler);

    start_udp(SERVER_NAME, PORT_TCP);

    g_server = start_server(atoi(argv[2]));
    if (!g_server) {
        fprintf(stderr, "Erreur lors du démarrage du serveur\n");
        exit(EXIT_FAILURE);
    }
    
    init_server_state(g_server);
    add_sqlite_to_server(g_server);
    init_db(g_server);
    
    server_state *state = get_server_state();
    while(!state->should_stop){
        server_client_procedure(g_server);
        usleep(100000);
    }

    cleanup_all_resources();
    close_debug_log();
    
    return EXIT_SUCCESS;
}