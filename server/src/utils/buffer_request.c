#include <string.h>
#include <stdlib.h>
#include "buffer_requests.h"

static char is_valid_char(char letter) {
    return (letter >= 'A' && letter <= 'Z') || 
           (letter >= 'a' && letter <= 'z') || 
           letter == '/' || letter == ' ' || letter == '_';
}

static int find_route_end(buffer *b) {
    if (!b) return -1;
    for (unsigned int i = 0; i < b->size; i++) {
        if (!is_valid_char(b->buffer[i])) {
            return i;
        }
    }
    return b->size;
}


static int find_newline_pos(buffer *b, int start_pos) {
    if (!b || start_pos < 0) return -1;
    for (unsigned int i = start_pos; i < b->size; i++) {
        if (b->buffer[i] == '\n') {
            return i;
        }
    }
    return -1;
}

static int find_json_start(buffer *b, int start_pos) {
    if (!b || start_pos < 0) return -1;
    for (unsigned int i = start_pos; i < b->size; i++) {
        if (b->buffer[i] == '{') {
            return i;
        }
    }
    return -1;
}


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

    // Vérifier si c'est une requête GET
    if (strncmp(b->buffer, "GET ", 4) == 0) {
        // Pour GET, extraire jusqu'à la fin de la route
        int route_end = find_route_end(b);
        if (route_end <= 4) {
            return NULL; // Pas de route valide
        }
        
        char *request = malloc((route_end + 1) * sizeof(char));
        if (!request) {
            return NULL;
        }
        strncpy(request, b->buffer, route_end);
        request[route_end] = '\0';
        return request;
    }

    // Vérifier si c'est une requête POST
    if (strncmp(b->buffer, "POST ", 5) == 0) {
        // Trouver la fin de la route
        int route_end = find_route_end(b);
        if (route_end <= 5) {
            return NULL; // Pas de route valide
        }

        // Vérifier s'il y a un newline après la route
        int newline_pos = find_newline_pos(b, route_end);
        
        // Si pas de newline, la requête est juste la route
        if (newline_pos == -1) {
            char *request = malloc((route_end + 1) * sizeof(char));
            if (!request) {
                return NULL;
            }
            strncpy(request, b->buffer, route_end);
            request[route_end] = '\0';
            return request;
        }

        // Si il y a un newline, chercher le JSON
        int json_start = find_json_start(b, newline_pos + 1);
        if (json_start == -1) {
            // Pas de JSON après le newline, la requête n'est pas complète
            return NULL;
        }

        // Trouver la fin du JSON
        int json_end = find_json_end(b, json_start);
        if (json_end == -1) {
            return NULL; // JSON incomplet
        }

        // Extraire la requête complète (route + newline + JSON)
        int request_len = json_end + 1;
        char *request = malloc((request_len + 1) * sizeof(char));
        if (!request) {
            return NULL;
        }
        strncpy(request, b->buffer, request_len);
        request[request_len] = '\0';
        return request;
    }

    // Si ça ne commence ni par GET ni par POST
    return NULL;
}

char request_available(buffer *b){
    if (!b || b->size == 0) {
        return 0;
    }

    // Verifier que le buffer commence par GET ou POST
    if (b->size < 4) {
        return 0;
    }

    // Verifier si c'est une requête GET
    if (strncmp(b->buffer, "GET ", 4) == 0) {
        // Pour GET, vérifier qu'il y a au moins une route valide
        int route_end = find_route_end(b);
        return (route_end > 4) ? 1 : 0;
    }

    // Verifier si c'est une requête POST
    if (strncmp(b->buffer, "POST ", 5) == 0) {
        // Trouver la fin de la route
        int route_end = find_route_end(b);
        if (route_end <= 5) {
            return 0; // Pas de route valide
        }

        // Vérifier s'il y a un newline après la route
        int newline_pos = find_newline_pos(b, route_end);
        
        // Si pas de newline, la requête est complète (juste la route)
        if (newline_pos == -1) {
            return 1;
        }

        // Si il y a un newline, vérifier qu'il y a un corps JSON complet
        int json_start = find_json_start(b, newline_pos + 1);
        if (json_start == -1) {
            // Pas de JSON après le newline, la requête n'est pas complète
            return 0;
        }

        // Compter les accolades pour verifier que le JSON est complet
        int open_braces = 0;
        int close_braces = 0;
        count_braces(b, json_start, &open_braces, &close_braces);

        // Il faut au moins 1 paire d'accolades et autant d'ouvrantes que de fermantes
        if (open_braces >= 1 && open_braces == close_braces) {
            return 1;
        }

        return 0;
    }

    // Si ça ne commence ni par GET ni par POST
    b->size = 0;
    return 0;
}

void update_buffer(buffer *b, unsigned int size){
    if (!b || size == 0) {
        return;
    }

    if (size >= b->size) {
        b->size = 0;
        b->buffer[0] = '\0';
        return;
    }

    memmove(b->buffer, b->buffer + size, b->size - size);
    
    b->size -= size;
    
    b->buffer[b->size] = '\0';
}

void reset_full_buffer(buffer *b){
    if (!b) return;
    b->size = 0;
    b->buffer[0] = '\0';
}