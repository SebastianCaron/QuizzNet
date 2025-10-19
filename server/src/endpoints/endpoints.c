#include <string.h>

#include "../network/client.h"
#include "endpoints.h"

char is_valid_char(char letter){
    return (letter >= 'A' && letter <= 'Z') || (letter >= 'a' && letter <= 'z') || letter == '/';
}

void handle_request(char *requets, client *cl){
    switch (get_endpoint(requets))
    {
    case POST_PLAYER_REGISTER:
        /* code */
        break;
    
    case INVALID_ENDPOINT:
        send_invalid_response(cl);
        break;
    
    default:
        send_error_response(cl);
        break;
    }
}

endpoints get_endpoint(char *request){
    if(!request) return INVALID_ENDPOINT;

    int i = 0;
    while(is_valid_char(request[i])){
        i++;
    }

    if(strncmp("POST player/register", request, i)) return POST_PLAYER_REGISTER;
    
    return INVALID_ENDPOINT;
}

void send_invalid_response(client *cl){

}

void send_error_response(client *cl){

}