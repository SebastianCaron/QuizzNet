#pragma once

#include "../game_logic/session_objects.h"

typedef struct {
    int score;
    char joker_pass;
    char joker_5050;
    char is_creator;
    session *session;
} infos_session_client;

typedef struct {
    int fd;
    char connected;
    char* pseudo;
    infos_session_client infos_session;
} client;

client *client_init(int fd);

void client_destroy(client *c);