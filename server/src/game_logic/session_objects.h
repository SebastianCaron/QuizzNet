#pragma once

#include "../network/server.h"
#include "../utils/chained_list.h"


typedef enum{
    EASY,
    MEDIUM,
    HARD
} difficulty;


typedef enum{
    CLASSIC,
    BATTLE
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
    session_type type;
    int nb_lives;
    int nb_players;
    int max_nb_players;
    session_status status;

    chained_list *players;
    server *server;
} session;