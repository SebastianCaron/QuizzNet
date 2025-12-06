#pragma once

#include <pthread.h>

/**
 * @file udp.h
 * @brief UDP server discovery module.
 * 
 * This module implements a UDP-based server discovery mechanism.
 * Clients can broadcast discovery requests, and the server responds
 * with its name and TCP port for connection.
 * 
 * @authors
 * Sebastian Caron
 * Lina Lethoor
 */

/**
 * @def UDP_PORT
 * @brief Default port for UDP discovery broadcasts.
 */
#define UDP_PORT 5555

/**
 * @def BUFFER_SIZE
 * @brief Size of the buffer used for UDP message exchange.
 */
#define BUFFER_SIZE 1024

/**
 * @struct udp_thread_args
 * @brief Arguments passed to the UDP discovery thread.
 * 
 * @param server_name Name of the server to advertise.
 * @param port_tcp TCP port the server is listening on.
 */
typedef struct {
    const char *server_name;
    const char *port_tcp;
} udp_thread_args;

/**
 * @brief Starts the UDP discovery service in a background thread.
 * 
 * Creates a new thread that listens for UDP discovery requests.
 * When a client sends "looking for quiznet servers", the server
 * responds with its name and TCP port.
 * 
 * @param SERVER_NAME Name of the server to advertise to clients.
 * @param PORT_TCP TCP port that clients should connect to.
 * @return The pthread_t handle of the created UDP thread.
 */
pthread_t start_udp(const char *SERVER_NAME, const char *PORT_TCP);
