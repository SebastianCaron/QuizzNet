#pragma once

#include "../network/client.h"
#define MAX_ENDPOINT_NAME 256

typedef enum {
    INVALID_ENDPOINT,
    POST_PLAYER_REGISTER,
    POST_PLAYER_LOGIN,
    POST_SESSION_CREATE,
    POST_SESSION_START,
    GET_SESSION_LIST,
    GET_THEMES_LIST,
    POST_JOKER_USE,
    POST_QUESTION_ANSWER
} endpoints;

endpoints get_endpoint(char *request);
void handle_request(server *s, char *request, client *cl);

void send_invalid_response(client *cl);
void send_error_response(client *cl);
void send_response(client *cl, char *response);