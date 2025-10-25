#pragma once

#include "../network/client.h"
#define MAX_ENDPOINT_NAME 256

typedef enum {
    INVALID_ENDPOINT,
    POST_PLAYER_REGISTER,
    POST_SESSION_CREATE,
    POST_SESSION_START
} endpoints;

void handle_request(server *s, char *request, client *cl);

void send_invalid_response(client *cl);
void send_error_response(client *cl);
void send_response(client *cl, char *response);