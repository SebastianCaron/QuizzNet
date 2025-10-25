#pragma once

typedef struct {
    int fd;
    bool connected;
    char* pseudo;
    infos_session_client infos_session;
} client;

typedef struct {
    int score;
    bool joker_pass;
    bool joker_5050;
} infos_session_client;

client *client_init(int fd);

void client_destroy(client *c);