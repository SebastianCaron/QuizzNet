#pragma once

#include "server.h"
#include "client.h"
#include "../utils/buffer_requests.h"


/**
 * @file network.h
 * @brief TCP server networking module.
 * 
 * This module handles TCP server operations including starting the server,
 * accepting client connections, receiving data, and managing the main
 * server loop for processing client requests.
 * 
 * @authors
 * Sebastian Caron
 * Lina Lethoor
 */

/**
 * @def MAX_CLIENTS
 * @brief Maximum number of simultaneous client connections.
 */
#define MAX_CLIENTS 512

/**
 * @def BUFFER_SIZE
 * @brief Default buffer size for receiving network data.
 */
#define BUFFER_SIZE 1024

/**
 * @brief Receives data from a file descriptor into a buffer.
 * 
 * Appends received data to the buffer's existing content.
 * The buffer capacity is initialized to MAX_BUFFER_SIZE if not set.
 * 
 * @param fd File descriptor to read from.
 * @param b Pointer to the buffer structure for storing received data.
 * @return Number of bytes in buffer after receive, -1 if connection closed, -2 on error.
 */
int receive_from(int fd, buffer *b);

/**
 * @brief Creates and starts the TCP server on the specified port.
 * 
 * Initializes the server structure, creates the TCP socket, binds to
 * the port, and starts listening for connections.
 * 
 * @param port Port number to listen on.
 * @return Pointer to the created server structure, or NULL on failure.
 */
server *start_server(int port);

/**
 * @brief Frees all resources associated with the server.
 * 
 * Destroys the client list, session list, and frees allocated memory.
 * Note: Does not close individual client connections.
 * 
 * @param s Pointer to the server to destroy.
 */
void destroy_server(server *s);

/**
 * @brief Main server loop iteration for handling client connections.
 * 
 * Accepts new connections and processes data from all connected clients.
 * Should be called repeatedly in the main server loop.
 * 
 * @param s Pointer to the server structure.
 */
void server_client_procedure(server *s);

/**
 * @brief Removes a client from the server's client list.
 * 
 * @param s Pointer to the server.
 * @param cl Pointer to the client to remove.
 */
void remove_client_from_server_procedure(server *s, client *cl);

/**
 * @brief Adds a client to the server's client list.
 * 
 * The client is added to the server's client list.
 * It means that the server will try to read in the client fd and will handle requests from it.
 * 
 * @param s Pointer to the server.
 * @param cl Pointer to the client to add.
 */
void attach_client_to_server_procedure(server *s, client *cl);

/**
 * @brief Initializes the global server state.
 * 
 * @param s Pointer to the server structure to associate with the state.
 */
void init_server_state(server *s);

/**
 * @brief Signals the server to stop.
 * 
 * Sets the should_stop flag to 1, causing all loops to exit gracefully.
 */
void cleanup_server_state(void);

/**
 * @brief Returns a pointer to the global server state.
 * 
 * @return Pointer to the server_state structure.
 */
server_state* get_server_state(void);
