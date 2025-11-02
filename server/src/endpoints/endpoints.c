#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../network/client.h"
#include "../network/network.h"
#include "endpoints.h"

/* ENDPOINTS */
#include "./player/register_player.h"
#include "./player/login_player.h"
#include "./session/create_session.h"
#include "./session/start_session.h"

char is_valid_char(char letter){
    return (letter >= 'A' && letter <= 'Z') || (letter >= 'a' && letter <= 'z') || letter == '/' || letter == ' ' || letter == '_';
}

endpoints get_endpoint(char *request){
    if(!request) return INVALID_ENDPOINT;

    int i = 0;
    while(is_valid_char(request[i])){
        i++;
    }
    i--;
    // ADD ENDPOINT ROUTE FROM HERE
    if(!strncmp("POST player/register", request, i)) return POST_PLAYER_REGISTER;
    if(!strncmp("POST player/login", request, i)) return POST_PLAYER_LOGIN;
    if(!strncmp("POST session/create", request, i)) return POST_SESSION_CREATE;
    if(!strncmp("POST session/start", request, i)) return POST_SESSION_CREATE;
    
    return INVALID_ENDPOINT;
}

void send_invalid_response(client *cl){
    char *response = 
    "{\n"
    "   \"statut\":\"400\",\n"
    "   \"message\": \"Bad request\"\n"
    "}\n";
    send_response(cl, response);
}

void send_error_response(client *cl){
    char *response = 
    "{\n"
    "   \"statut\":\"520\",\n"
    "   \"message\": \"Unknown Error\"\n"
    "}\n";
    send_response(cl, response);
}

void send_response(client *cl, char *response){
    if (!cl || !response) return;
    write(cl->fd, response, strlen(response));
}


void handle_request(server *s, char *request, client *cl){
    endpoints ep = get_endpoint(request);
    debug_log("ENDPOINTS: %d\n", ep);
    // HANDLE ROUTE HERE
    switch (ep)
    {
    case POST_PLAYER_REGISTER:
        post_player_register(s, request, cl);
        break;
    case POST_PLAYER_LOGIN:
        post_player_login(s, request, cl);
    case POST_SESSION_CREATE:
        post_session_create(s, request, cl);
        break;
    
    case INVALID_ENDPOINT:
        send_invalid_response(cl);
        break;
    
    default:
        send_error_response(cl);
        break;
    }
}
