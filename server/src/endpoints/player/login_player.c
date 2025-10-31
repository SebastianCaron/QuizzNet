#include <stdio.h>

#include "login_player.h"

#include "../../network/client.h"
#include "../../network/network.h"
#include "../endpoints.h"
#include "../../db/db.h"
#include "../../json/json.h"

int post_player_login(server *s, char *request, client *cl){
    char query[1024] = {'\0'};

    cJSON *json = cJSON_Parse(request);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            throw_error(JSON_PARSING, error_ptr);
        }
        cJSON_Delete(json);
        return 1;
    }

    // TODO: Hash password
    snprintf(query, 1023, "SELECT c.password FROM clients c WHERE c.pseudo = %s;", 
        get_from_json_string(json, "pseudo"));

    SqliteResult *res = exec_query(s, query);
    if(!res){
        char *response =
        "{"
        "   \"action\":\"player/login\",\n"
        "   \"statut\":\"401\",\n"
        "   \"message\":\"invalid credentials : Unknown Pseudo\"\n"
        "}";
        throw_error(DB_QUERY, "Erreur pseudo post_player_login");
        send_esponse(cl, response);
        cJSON_Delete(json);
        return 1;
    }

    if(strcmp(res->rows[0][0], get_from_json_string(json, "password"))){
        char *response =
        "{"
        "   \"action\":\"player/login\",\n"
        "   \"statut\":\"401\",\n"
        "   \"message\":\"invalid credentials : Wrong Password\"\n"
        "}";
        throw_error(DB_QUERY, "Erreur password post_player_login");
        send_esponse(cl, response);
        cJSON_Delete(json);
        return 1;
    }
    
    char *response =
    "{"
    "   \"action\":\"player/login\",\n"
    "   \"statut\":\"200\",\n"
    "   \"message\":\"login successful\"\n"
    "}";
    send_response(cl, response);
    cJSON_Delete(json);
    return 0;
}