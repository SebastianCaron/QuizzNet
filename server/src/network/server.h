#pragma once

#include <arpa/inet.h>
#include <pthread.h>

#include "../utils/chained_list.h"
#include "../sqlite/sqlite3.h"

/**
 * @file server.h
 * @brief Server structure definitions.
 * 
 * This file contains the main data structures representing the server
 * and its global state, including client connections, game sessions,
 * and network configuration.
 * 
 * @authors
 * Sebastian Caron
 * Lina Lethoor
 */

/**
 * @struct server
 * @brief Main server structure holding all server resources.
 * 
 * @param ip Server IP address (optional, for display purposes).
 * @param port Server listening port.
 * @param server_fd_tcp TCP socket file descriptor for accepting connections.
 * @param address Socket address structure for the server.
 * @param clients Linked list of connected clients (client*).
 * @param sessions Linked list of active game sessions (session*).
 * @param session_counter Counter for generating unique session IDs.
 * @param buffer Shared buffer for receiving client messages.
 * @param size_buffer Current allocated size of the buffer.
 * @param current_size Number of bytes currently in the buffer.
 * @param db SQLite database handle for persistent storage.
 */
typedef struct {
    char *ip;
    int port;

    int server_fd_tcp;

    struct sockaddr_in address;

    chained_list *clients;
    chained_list *sessions;
    int session_counter;

    char *buffer;
    int size_buffer;
    ssize_t current_size;

    sqlite3 *db;

} server;

/**
 * @struct server_state
 * @brief Global server state for coordinating shutdown and threads.
 * 
 * @param should_stop Flag indicating the server should stop (1 = stop, 0 = running).
 * @param udp_thread Handle to the UDP discovery thread.
 * @param srv Pointer to the main server structure.
 */
typedef struct {
    int should_stop;
    pthread_t udp_thread;
    server *srv;
} server_state;
