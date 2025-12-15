#include <stdio.h>
#include <string.h>

#include "login_player.h"

#include "../endpoints.h"
#include "../../db/db.h"
#include "../../json/json.h"
#include "../../utils/hash_pass.h"

int post_player_login(server *s, char *request, client *cl){
    char query[1024] = {'\0'};

    /* Skip to JSON body */
    while(request && (request[0] != '{' && request[0] != '\0')) request++;

    cJSON *json = cJSON_Parse(request);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            throw_error(JSON_PARSING, error_ptr);
        }
        send_error_response(cl);
        return 1;
    }
    
    /* Extract credentials */
    char *pseudo = get_from_json_string(json, "pseudo");
    char *password = get_from_json_string(json, "password");

    if (!pseudo || !password) {
        throw_error(JSON_PARSING, "pseudo or password missing");
        send_error_response(cl);
        return 1;
    }

    /* Query database for user's password hash */
    snprintf(query, 1023, "SELECT password FROM clients WHERE pseudo = '%s';", pseudo);

    SqliteResult *res = exec_query(s, query);
    if(!res){
        throw_error(DB_QUERY, "Erreur post_player_login");
        cJSON_Delete(json);
        send_error_response(cl);
        return 1;
    }

    /* Check if user exists */
    if(res->row_count == 0){
        char *response =
        "{"
        "   \"action\":\"player/login\",\n"
        "   \"statut\":\"401\",\n"
        "   \"message\":\"invalid credentials\"\n"
        "}\n\n";
        cJSON_Delete(json);
        sqlite_result_destroy(res);
        send_response(cl, response);
        return 1;
    }

    /* Verify password hash */
    char *hash_pass = (char *) hash_password(password);
    if(strcmp(res->rows[0][0], hash_pass)){
        char *response =
        "{"
        "   \"action\":\"player/login\",\n"
        "   \"statut\":\"401\",\n"
        "   \"message\":\"invalid credentials\"\n"
        "}\n\n";
        free(hash_pass);
        cJSON_Delete(json);
        send_response(cl, response);
        sqlite_result_destroy(res);
        return 1;
    }

    cl->pseudo = strdup(pseudo);
    /* Cleanup */
    free(hash_pass);
    sqlite_result_destroy(res);
    cJSON_Delete(json);
    
    /* Send success response */
    char *response =
    "{"
    "   \"action\":\"player/login\",\n"
    "   \"statut\":\"200\",\n"
    "   \"message\":\"login successful\"\n"
    "}\n\n";
    send_response(cl, response);
    return 0;
}
