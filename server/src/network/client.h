#pragma once

typedef struct {
    int fd;
} client;


client *client_init(int fd);

void client_destroy(client *c);