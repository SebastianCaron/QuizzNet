#pragma once

#include "../game_logic/session_objects.h"
#include "../utils/buffer_requests.h"

#define NB_LIVES 4

typedef struct {
    char answer_text[512];
    char answer_other;
    float response_time;
} answer;

typedef struct {
    int score;
    char joker_pass;
    char joker_5050;
    char is_creator;
    session *session;
    char has_answered;
    char skip;
    char lives;
    int correctAnswers;
    int eliminatedAt;

    answer player_answer;
} infos_session_client;

typedef struct {
    int fd;
    char connected;
    char* pseudo;
    infos_session_client infos_session;
    buffer buffer_cl;
} client;

client *client_init(int fd);

void client_destroy(client *c);