#pragma once

#define MAX_BUFFER_SIZE 4096

typedef struct {
    char buffer[MAX_BUFFER_SIZE + 1];
    unsigned int capacity;
    unsigned int size;
} buffer;

char *get_request(buffer *b);

char request_available(buffer *b);

void update_buffer(buffer *b, unsigned int size);

void reset_full_buffer(buffer *b);