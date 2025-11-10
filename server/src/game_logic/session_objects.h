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

typedef enum{
    QCM,
    TRUEFALSE,
    FREETEXT
} question_type;

typedef struct {
    int id;
    question_type type;
    difficulty diff;
    char statement[500];
    char answer[500];
} question;

typedef struct {

    int id;
    char *name;
    int *themes_ids;
    int nb_themes;
    difficulty difficulty;
    int nb_questions;
    int time_limit;
    session_type type;
    int nb_lives;
    int nb_players;
    int max_nb_players;
    session_status status;

    char *buffer;
    int buffer_capacity;
    ssize_t buffer_size;

    chained_list *players;
    server *server;
    
    question current_question;
    int correct_answer_index;
} session;