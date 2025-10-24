#pragma once

#include "../network/client.h"
#define MAX_ENDPOINT_NAME 256

typedef enum {
    INVALID_ENDPOINT,
    POST_PLAYER_REGISTER
} endpoints;

void handle_request(server *s, char *requets, client *cl);

void send_invalid_response(client *cl);
void send_error_response(client *cl);
void send_response(client *cl, char *response);