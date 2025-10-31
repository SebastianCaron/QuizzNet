#include <stdio.h>

#include "register_player.h"

#include "../../network/client.h"
#include "../../network/network.h"
#include "../endpoints.h"
#include "../../db/db.h"
#include "../../json/json.h"

int post_player_register(server *s, char *request, client *cl){
    char query[1024] = {'\0'};
    char query_pseudo[1024] = {'\0'};

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
    
    // Vérifier que le joueur n'existe pas déjà
    snprintf(query, 1023, "SELECT id FROM clients WHERE pseudo = '%s';", pseudo);
    SqliteResult *check_res = exec_query(s, query);
    if(!check_res){
        throw_error(DB_QUERY, "Erreur lors de la vérification du joueur");
        cJSON_Delete(json);
        send_error_response(cl);
        return 1;
    }
    
    if(check_res->row_count > 0){
        char *response =
        "{"
        "   \"action\":\"player/register\",\n"
        "   \"statut\":\"409\",\n"
        "   \"message\":\"pseudo already exists\"\n"
        "}";
        sqlite_result_destroy(check_res);
        cJSON_Delete(json);
        send_response(cl, response);
        return 1;
    }
    
    sqlite_result_destroy(check_res);
    
    // TODO: Hash password

    snprintf(query, 1023, "INSERT INTO clients(pseudo, password) VALUES ('%s', '%s');", 
        pseudo, password);
    SqliteResult *res = exec_query(s, query);
    if(!res){
        throw_error(DB_QUERY, "Erreur post_player_register");
        send_error_response(cl);
        return 1;
    }
    cJSON_Delete(json);
    sqlite_result_destroy(res);

    char *response =
    "{"
    "   \"action\":\"player/register\",\n"
    "   \"statut\":\"201\",\n"
    "   \"message\":\"player registered successfully\"\n"
    "}";
    send_response(cl, response);

    return 0;
}