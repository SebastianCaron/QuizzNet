#pragma once

typedef struct {
    int fd;
    char connected;
    char* pseudo;
    infos_session_client infos_session;
} client;

typedef struct {
    int score;
    char joker_pass;
    char joker_5050;
} infos_session_client;

client *client_init(int fd);

void client_destroy(client *c);