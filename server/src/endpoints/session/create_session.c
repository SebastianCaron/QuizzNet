#include <string.h>
#include "create_session.h"
#include "../endpoints.h"
#include "../../network/network.h"
#include "../../json/json.h"
#include "../../utils/chained_list.h"
#include "../../game_logic/session.h"

int post_session_create(server* s, char* request, client *cl){
    int retour_snp;
    char response[1024] = {'\0'};

    /* Allocate new session */
    session* new_session = malloc(sizeof(session));
    if(!new_session){
        throw_error(MEMORY_ALLOCATION, "Erreur allocation session_create : new session");
        return 1;
    }

    while(request && (request[0] != '{' && request[0] != '\0')) request++;
    /* Parse JSON request */
    cJSON *json = cJSON_Parse(request);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            throw_error(JSON_PARSING, error_ptr);
        }
        cJSON_Delete(json);
        free(new_session);
        return 1;
    }
    
    /* Initialize session from JSON parameters */
    new_session->id = s->session_counter++;
    new_session->name = get_from_json_string(json, "name");
    new_session->themes_ids = get_from_json_int_array(json, "themeIds");
    new_session->difficulty = get_session_difficulty(get_from_json_string(json, "difficulty"));
    new_session->nb_questions = get_from_json_int(json, "nbQuestions");
    new_session->time_limit = get_from_json_int(json, "timeLimit");
    new_session->type = get_session_type(get_from_json_string(json, "mode"));
    new_session->nb_lives = new_session->type == BATTLE ? 4 : 0;
    new_session->nb_players = 1;  /* Creator counts as first player */
    new_session->max_nb_players = get_from_json_int(json, "maxPlayers");
    new_session->status = WAITING;

    /* Initialize players list */
    new_session->players = clist_init();
    if(!(new_session->players)){
        throw_error(MEMORY_ALLOCATION, "Erreur allocation session_create : new_session -> client");
        cJSON_Delete(json);
        free(new_session);
        return 1;
    }
    
    /* Remove client from previous session if any */
    if(cl->infos_session.session){
        session_remove_client(cl->infos_session.session, cl);
    }
    
    /* Add creator to session */
    clist_append(new_session->players, cl);
    cl->infos_session.session = new_session;
    cl->infos_session.is_creator = 1;
    new_session->server = s;
    
    /* Register session with server */
    clist_append(s->sessions, new_session);

    /* Build response based on game mode */
    if (new_session->type == CLASSIC){
        retour_snp = snprintf(response, 1023, "{"
        "   \"action\":\"session/create\",\n"
        "   \"statut\":\"201\",\n"
        "   \"message\":\"session created\",\n"
        "   \"sessionId\": %d,\n"
        "   \"isCreator\": true,\n"
        "   \"jokers\":{\n"
        "      \"fifty\": 1,\n"
        "      \"skip\": 1\n"
        "   }\n"
        "}", new_session->id);

        if (retour_snp < 0){
            throw_error(ENCODING_ERROR, "Erreur snprintf create session CLASSIC");
            return 1;
        }

    } else {
        /* BATTLE mode includes lives count */
        retour_snp = snprintf(response, 1023, "{"
        "   \"action\":\"session/create\",\n"
        "   \"statut\":\"201\",\n"
        "   \"message\":\"session created\",\n"
        "   \"sessionId\": %d, \n"
        "   \"isCreator\": true,\n"
        "   \"lives\": %d, \n"
        "   \"jokers\":{\n"
        "      \"fifty\": 1,\n"
        "      \"skip\": 1\n"
        "   }"
        "}", new_session->id, new_session->nb_lives);

        if (retour_snp < 0){
            throw_error(ENCODING_ERROR, "Erreur snprintf create session BATTLE");
            return 1;
        }
    }

    send_response(cl, response);

    return 0;
}
