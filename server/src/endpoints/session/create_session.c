#include "create_session.h"
#include "../../endpoints.h"
#include "../../../network/network.h"
#include "../json/json.h"
#include "../../network/client.h"

int post_session_create(server* s, char* request, client *cl){
    int retour_snp;
    const char *response = malloc(1024*sizeof(char));
    session* new_session = malloc(sizeof(session))
    if(!new_session){
        throw_error(MEMORY_ALLOCATION, "Erreur allocation session_create : new session");
        return NULL;
    }

    cJSON *json = cJSON_Parse(request);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            printf("Error: %s\n", error_ptr);
        }
        cJSON_Delete(json);
        return 1;
    }
    
    new_session->id = s->session_counter++;
    new_session->name = get_from_json_string(json, "name");
    new_session->themes_ids = get_from_json_int_array(json, "themesIds");
    new_session->difficulty = get_session_difficulty(json, "difficulty");
    new_session->nb_questions = get_from_json_int(json, "nbQuestions");
    new_session->time_limit = get_from_json_int(json, "timeLimit");
    new_session->type = get_session_type(get_from_json_string(json, "mode"));
    new_session->nb_lives = new_session->type == session_type.BATTLE ? 4 : 0;
    new_session->nb_players = 1; // 1 = the creator of the session. When someone disconnect, -1
    new_session->max_nb_players = get_from_json_int(json, "maxPlayers");
    new_session->status = session_status.WAITING;
    new_session->players = malloc(new_session->max_nb_players * sizeof(client));
    if(!(new_session->players)){
        throw_error(MEMORY_ALLOCATION, "Erreur allocation session_create : new_session -> client");
        return NULL;
    }
    new_session->players[0] = cl;
    new_session->server = s;

    if (new_session->type == session_type.CLASSIC){
        retour_snp = snprintf(response,"{"
        "   \"action\":\"session/create\",\n"
        "   \"statut\":\"201\",\n"
        "   \"message\":\"session created\",\n"
        "   \"sessionId\": %d, \n"
        "   \"isCreator\": true,\n"
        "   \"jokers\":{\n"
        "      \"fifty\": 1,\n"
        "      \"skip\": 1\n"
        "   }"
        "}", new_session->id);

        if (retour_snp<0){
            throw_error(ENCODING_ERROR, "Erreur snprintf create session CLASSIC");
            return NULL;
        }

    } else {
        retour_snp = snprintf(response,"{"
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

        if (retour_snp<0){
            throw_error(ENCODING_ERROR, "Erreur snprintf create session BATTLE");
            return NULL;
        }
    }

    send_response(cl, response);

    return 0;
}