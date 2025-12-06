#include <string.h>
#include <stdlib.h>
#include "buffer_requests.h"

/**
 * @brief Checks if a character is valid for a route path.
 * 
 * Valid characters: A-Z, a-z, '/', ' ', '_'
 * 
 * @param letter Character to check.
 * @return 1 if valid, 0 otherwise.
 */
static char is_valid_char(char letter) {
    return (letter >= 'A' && letter <= 'Z') || 
           (letter >= 'a' && letter <= 'z') || 
           letter == '/' || letter == ' ' || letter == '_';
}

/**
 * @brief Finds the end of the route in the buffer.
 * 
 * Scans until a non-valid route character is found.
 * 
 * @param b Pointer to the buffer.
 * @return Index of first invalid character, or buffer size if all valid.
 */
static int find_route_end(buffer *b) {
    if (!b) return -1;
    for (unsigned int i = 0; i < b->size; i++) {
        if (!is_valid_char(b->buffer[i])) {
            return i;
        }
    }
    return b->size;
}

/**
 * @brief Finds the position of a newline character starting from a given position.
 * 
 * @param b Pointer to the buffer.
 * @param start_pos Starting position for the search.
 * @return Index of newline, or -1 if not found.
 */
static int find_newline_pos(buffer *b, int start_pos) {
    if (!b || start_pos < 0) return -1;
    for (unsigned int i = start_pos; i < b->size; i++) {
        if (b->buffer[i] == '\n') {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Finds the start of a JSON object (opening brace).
 * 
 * @param b Pointer to the buffer.
 * @param start_pos Starting position for the search.
 * @return Index of '{', or -1 if not found.
 */
static int find_json_start(buffer *b, int start_pos) {
    if (!b || start_pos < 0) return -1;
    for (unsigned int i = start_pos; i < b->size; i++) {
        if (b->buffer[i] == '{') {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Counts opening and closing braces in the buffer.
 * 
 * Used to verify JSON completeness by checking brace balance.
 * 
 * @param b Pointer to the buffer.
 * @param json_start Starting position (should be at '{').
 * @param open_braces Output: count of '{' characters.
 * @param close_braces Output: count of '}' characters.
 */
static void count_braces(buffer *b, int json_start, int *open_braces, int *close_braces) {
    if (!b || !open_braces || !close_braces || json_start < 0) {
        *open_braces = 0;
        *close_braces = 0;
        return;
    }
    *open_braces = 0;
    *close_braces = 0;
    for (unsigned int i = json_start; i < b->size; i++) {
        if (b->buffer[i] == '{') {
            (*open_braces)++;
        } else if (b->buffer[i] == '}') {
            (*close_braces)++;
        }
    }
}

/**
 * @brief Finds the end of a JSON object (matching closing brace).
 * 
 * Tracks brace nesting to find the matching '}' for the opening '{'.
 * 
 * @param b Pointer to the buffer.
 * @param json_start Starting position (should be at '{').
 * @return Index of matching '}', or -1 if JSON is incomplete.
 */
static int find_json_end(buffer *b, int json_start) {
    if (!b || json_start < 0) return -1;
    int open_braces = 0;
    for (unsigned int i = json_start; i < b->size; i++) {
        if (b->buffer[i] == '{') {
            open_braces++;
        } else if (b->buffer[i] == '}') {
            open_braces--;
            if (open_braces == 0) {
                return i;
            }
        }
    }
    return -1;
}

char *get_request(buffer *b){
    if (!b || b->size == 0) {
        return NULL;
    }

    /* Check if it's a GET request */
    if (strncmp(b->buffer, "GET ", 4) == 0) {
        /* For GET, extract until end of route */
        int route_end = find_route_end(b);
        if (route_end <= 4) {
            return NULL;  /* No valid route */
        }
        
        char *request = malloc((route_end + 1) * sizeof(char));
        if (!request) {
            return NULL;
        }
        strncpy(request, b->buffer, route_end);
        request[route_end] = '\0';
        return request;
    }

    /* Check if it's a POST request */
    if (strncmp(b->buffer, "POST ", 5) == 0) {
        /* Find end of route */
        int route_end = find_route_end(b);
        if (route_end <= 5) {
            return NULL;  /* No valid route */
        }

        /* Check for newline after route */
        int newline_pos = find_newline_pos(b, route_end);
        
        /* If no newline, request is just the route */
        if (newline_pos == -1) {
            char *request = malloc((route_end + 1) * sizeof(char));
            if (!request) {
                return NULL;
            }
            strncpy(request, b->buffer, route_end);
            request[route_end] = '\0';
            return request;
        }

        /* If newline present, look for JSON body */
        int json_start = find_json_start(b, newline_pos + 1);
        if (json_start == -1) {
            /* No JSON after newline, request not complete */
            return NULL;
        }

        /* Find end of JSON */
        int json_end = find_json_end(b, json_start);
        if (json_end == -1) {
            return NULL;  /* Incomplete JSON */
        }

        /* Extract complete request (route + newline + JSON) */
        int request_len = json_end + 1;
        char *request = malloc((request_len + 1) * sizeof(char));
        if (!request) {
            return NULL;
        }
        strncpy(request, b->buffer, request_len);
        request[request_len] = '\0';
        return request;
    }

    /* Neither GET nor POST */
    return NULL;
}

char request_available(buffer *b){
    if (!b || b->size == 0) {
        return 0;
    }

    /* Verify buffer starts with GET or POST */
    if (b->size < 4) {
        return 0;
    }

    /* Check if it's a GET request */
    if (strncmp(b->buffer, "GET ", 4) == 0) {
        /* For GET, verify there's at least a valid route */
        int route_end = find_route_end(b);
        return (route_end > 4) ? 1 : 0;
    }

    /* Check if it's a POST request */
    if (strncmp(b->buffer, "POST ", 5) == 0) {
        /* Find end of route */
        int route_end = find_route_end(b);
        if (route_end <= 5) {
            return 0;  /* No valid route */
        }

        /* Check for newline after route */
        int newline_pos = find_newline_pos(b, route_end);
        
        /* If no newline, request is complete (just the route) */
        if (newline_pos == -1) {
            return 1;
        }

        /* If newline present, verify complete JSON body exists */
        int json_start = find_json_start(b, newline_pos + 1);
        if (json_start == -1) {
            /* No JSON after newline, request not complete */
            return 0;
        }

        /* Count braces to verify JSON is complete */
        int open_braces = 0;
        int close_braces = 0;
        count_braces(b, json_start, &open_braces, &close_braces);

        /* Need at least 1 brace pair with balanced open/close */
        if (open_braces >= 1 && open_braces == close_braces) {
            return 1;
        }

        return 0;
    }

    /* Neither GET nor POST, reset buffer */
    b->size = 0;
    return 0;
}

void update_buffer(buffer *b, unsigned int size){
    if (!b || size == 0) {
        return;
    }

    /* If removing all or more data, just clear the buffer */
    if (size >= b->size) {
        b->size = 0;
        b->buffer[0] = '\0';
        return;
    }

    /* Shift remaining data to beginning of buffer */
    memmove(b->buffer, b->buffer + size, b->size - size);
    
    b->size -= size;
    
    /* Null-terminate */
    b->buffer[b->size] = '\0';
}

void reset_full_buffer(buffer *b){
    if (!b) return;
    b->size = 0;
    b->buffer[0] = '\0';
}
