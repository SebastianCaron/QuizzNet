#pragma once

/**
 * @file buffer_requests.h
 * @brief Request buffer management for parsing client messages.
 * 
 * This module provides utilities for buffering incoming data from clients
 * and parsing HTTP-like requests (GET/POST with optional JSON body).
 * 
 * @authors
 * Sebastian Caron
 * Lina Lethoor
 */

/**
 * @def MAX_BUFFER_SIZE
 * @brief Maximum size of the receive buffer per client.
 */
#define MAX_BUFFER_SIZE 4096

/**
 * @struct buffer
 * @brief Fixed-size buffer for accumulating incoming client data.
 * 
 * @param buffer Character array storing the raw data (null-terminated).
 * @param capacity Maximum capacity of the buffer (always MAX_BUFFER_SIZE).
 * @param size Current number of bytes stored in the buffer.
 */
typedef struct {
    char buffer[MAX_BUFFER_SIZE + 1];
    unsigned int capacity;
    unsigned int size;
} buffer;

/**
 * @brief Extracts a complete request from the buffer.
 * 
 * Parses the buffer content to extract a complete GET or POST request.
 * For GET: extracts "GET /route"
 * For POST: extracts "POST /route" or "POST /route\n{json}" if JSON body present.
 * 
 * @param b Pointer to the buffer.
 * @return Newly allocated string containing the request, or NULL if incomplete/invalid.
 * @note The caller is responsible for freeing the returned string.
 */
char *get_request(buffer *b);

/**
 * @brief Checks if a complete request is available in the buffer.
 * 
 * Validates that the buffer contains a complete parseable request:
 * - For GET: valid route present
 * - For POST: valid route, and if newline present, complete JSON body required
 * 
 * @param b Pointer to the buffer.
 * @return 1 if a complete request is available, 0 otherwise.
 */
char request_available(buffer *b);

/**
 * @brief Removes processed bytes from the beginning of the buffer.
 * 
 * Shifts remaining data to the start of the buffer after a request
 * has been processed.
 * 
 * @param b Pointer to the buffer.
 * @param size Number of bytes to remove from the beginning.
 */
void update_buffer(buffer *b, unsigned int size);

/**
 * @brief Resets the buffer to empty state.
 * 
 * Sets size to 0 and null-terminates the buffer.
 * 
 * @param b Pointer to the buffer to reset.
 */
void reset_full_buffer(buffer *b);
