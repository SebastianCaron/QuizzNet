#include <stdio.h>

#include "register_player.h"

#include "../../network/client.h"
#include "../../network/network.h"
#include "../endpoints.h"
#include "../../db/db.h"
#include "../../json/json.h"

int post_player_register(server *s, char *request, client *cl){
    char query[1024] = {'\0'};

    while(request && (request[0] != '{' && request[0] != '\0')) request++;
    cJSON *json = cJSON_Parse(request);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            throw_error(JSON_PARSING, error_ptr);
        }
        cJSON_Delete(json);
        return 1;
    }

    char *pseudo = get_from_json_string(json, "pseudo");
    char *password = get_from_json_string(json, "password");
    
    if (!pseudo || !password) {
        throw_error(JSON_PARSING, "pseudo or password missing");
        cJSON_Delete(json);
        send_error_response(cl);
        return 1;
    }
    
    snprintf(query, 1023, "INSERT INTO clients(pseudo, password) VALUES ('%s', '%s');", 
        pseudo, password);
    // TODO: Verifier que le joueur n'existe pas
    
    // TODO: Hash password

    
    SqliteResult *res = exec_query(s, query);
    if(!res){
        throw_error(DB_QUERY, "Erreur post_player_register");
        send_error_response(cl);
        return 1;
    }
    cJSON_Delete(json);

    char *response =
    "{"
    "   \"action\":\"player/register\",\n"
    "   \"statut\":\"201\",\n"
    "   \"message\":\"player registered successfully\"\n"
    "}";
    send_response(cl, response);

    return 0;
}