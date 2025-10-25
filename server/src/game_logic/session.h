#pragma once
#include "../network/client.h"
#include "../network/network.h"
#include "difficulty.h"


typedef enum{
    CLASSIC,
    BATTLE_ROYAL
} session_type;

typedef enum{
    WAITING,
    PLAYING,
    FINISHED
} session_status;

typedef struct {

    int id;
    char *name;
    int *themes_ids;
    difficulty difficulty;
    int nb_questions;
    int time_limit;
    int nb_players;
    int max_nb_players;
    session_status status;

    session_type type;
    client *players;
    server *s;
} session;

void *handle_session(void *session);