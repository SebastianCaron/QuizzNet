#include <stdio.h>
#include <string.h>

#include "login_player.h"

#include "../../network/client.h"
#include "../../network/network.h"
#include "../endpoints.h"
#include "../../db/db.h"
#include "../../json/json.h"

int post_player_login(server *s, char *request, client *cl){
    char query[1024] = {'\0'};

    while(request && (request[0] != '{' && request[0] != '\0')) request++;
    cJSON *json = cJSON_Parse(request);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            throw_error(JSON_PARSING, error_ptr);
        }
        cJSON_Delete(json);
        send_error_response(cl);
        return 1;
    }
    
    char *pseudo = get_from_json_string(json, "pseudo");
    char *password = get_from_json_string(json, "password");

    if (!pseudo || !password) {
        throw_error(JSON_PARSING, "pseudo or password missing");
        send_error_response(cl);
        return 1;
    }

    // TODO: Hash password
    snprintf(query, 1023, "SELECT password FROM clients WHERE pseudo = '%s';", 
       pseudo);

    SqliteResult *res = exec_query(s, query);
    if(!res){
        throw_error(DB_QUERY, "Erreur post_player_login");
        send_error_response(cl);
        return 1;
    }

    if(res->row_count == 0){
        sqlite_result_destroy(res);
        char *response =
        "{"
        "   \"action\":\"player/login\",\n"
        "   \"statut\":\"401\",\n"
        "   \"message\":\"invalid credentials\"\n"
        "}";
        cJSON_Delete(json);
        sqlite_result_destroy(res);
        send_response(cl, response);
        return 1;
    }

    if(strcmp(res->rows[0][0], password)){
        sqlite_result_destroy(res);
        char *response =
        "{"
        "   \"action\":\"player/login\",\n"
        "   \"statut\":\"401\",\n"
        "   \"message\":\"invalid credentials\"\n"
        "}";
        cJSON_Delete(json);
        send_response(cl, response);
        return 1;
    }
    
    sqlite_result_destroy(res);
    cJSON_Delete(json);
    
    char *response =
    "{"
    "   \"action\":\"player/login\",\n"
    "   \"statut\":\"200\",\n"
    "   \"message\":\"login successful\"\n"
    "}";
    send_response(cl, response);
    return 0;
}