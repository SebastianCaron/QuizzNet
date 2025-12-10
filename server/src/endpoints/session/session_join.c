#include <string.h>
#include "session_join.h"
#include "../endpoints.h"
#include "../../json/json.h"
#include "../../utils/chained_list.h"
#include "../../game_logic/session.h"

int post_session_join(server* s, char* request, client *cl){
    int retour_snp;
    int player_size;
    session* session_to_join;
    char* str_tmp;
    client* player;
    char response[1024] = {'\0'};
    char response_other_players[1024] = {'\0'};

    /* Parse JSON request */
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

    /* Find the session to join */
    session_to_join = get_session_by_id(s->sessions, get_from_json_int(json, "sessionId"));
    if(!session_to_join){
        char *response_full =
            "{"
            "   \"action\":\"session/join\",\n"
            "   \"statut\":\"404\",\n"
            "   \"message\":\"session does not exists\"\n"
            "}\n\n";
            send_response(cl, response_full);
            return 0;
    }

    /* Check if client is already in this session */
    if(cl->infos_session.session == session_to_join){
        char *response_full =
            "{"
            "   \"action\":\"session/join\",\n"
            "   \"statut\":\"400\",\n"
            "   \"message\":\"already in this session\"\n"
            "}\n\n";
        send_response(cl, response_full);
        cJSON_Delete(json);
        return 0;
    }
    
    /* Check if session is full */
    if (session_to_join->nb_players == session_to_join->max_nb_players){
            char *response_full =
            "{"
            "   \"action\":\"session/join\",\n"
            "   \"statut\":\"403\",\n"
            "   \"message\":\"session is full\"\n"
            "}\n\n";
            send_response(cl, response_full);
            cJSON_Delete(json);
            return 0;
    }
    
    /* Remove client from previous session if any */
    if(cl->infos_session.session){
        session_remove_client(cl->infos_session.session, cl);
    }
    
    /* Add client to session */
    clist_append(session_to_join->players, cl);
    session_to_join->nb_players++;
    
    /* Initialize player session info */
    cl->infos_session.joker_pass = 1;
    cl->infos_session.joker_5050 = 1;
    cl->infos_session.lives = session_to_join->nb_lives;
    cl->infos_session.score = 0;
    cl->infos_session.correctAnswers = 0;
    cl->infos_session.eliminatedAt = -1;
    cl->infos_session.is_creator = 0;

    /* Build notification for other players */
    retour_snp = snprintf(response_other_players, 1023, "POST session/player/joined\n{"
            "   \"pseudo\":\"%s\",\n"
            "   \"nbPlayers\":\"%d\"\n"
            "}\n\n", cl->pseudo, session_to_join->nb_players);

    if (retour_snp < 0){
        throw_error(ENCODING_ERROR, "Erreur snprintf join session response other players");
        return 1;
    }

    cl->infos_session.session = session_to_join;
    player_size = clist_size(session_to_join->players);

    /* Determine session type string */
    char *session_type_str = "";
    if(session_to_join->type == CLASSIC){
        session_type_str = "solo";
    } else {
        session_type_str = "battle";
    }

    /* Build response header */
    retour_snp = snprintf(response, 1023, "{"
        "   \"action\":\"session/join\",\n"
        "   \"statut\":\"201\",\n"
        "   \"message\":\"session joined\",\n"
        "   \"sessionId\": %d,\n"
        "   \"mode\":\"%s\",\n"
        "   \"isCreator\": false,\n"
        "   \"players\": [", session_to_join->id, session_type_str);
        
    if (retour_snp < 0){
        throw_error(ENCODING_ERROR, "Erreur snprintf join session");
        return 1;
    }

    /* Add player list and notify each player */
    for (int i = 0; i < player_size; i++){
        player = ((client *) (clist_get(session_to_join->players, i)));
        str_tmp = player->pseudo;
        retour_snp = snprintf(response + strlen(response), sizeof(response) - strlen(response), "\"%s\"", str_tmp);
        if (retour_snp < 0){
            throw_error(ENCODING_ERROR, "Erreur snprintf join session CLASSIC");
            return 1;
        }

        if((i + 1) != player_size) strcat(response, ",");

        /* Notify existing player about new player */
        if (strcmp(player->pseudo, cl->pseudo) != 0){
            send_response(player, response_other_players);
        }
    }

    /* Complete response based on game mode */
    if (session_to_join->type == CLASSIC){
        strcat(response, "],\n"
        "   \"jokers\":{\n"
        "      \"fifty\": 1,\n"
        "      \"skip\": 1\n"
        "   }\n"
        "}\n\n");
    } else {
        /* BATTLE mode includes lives */
        snprintf(response + strlen(response), sizeof(response) - strlen(response), "],\n"
        "   \"lives\": %d,\n"
        "   \"jokers\":{\n"
        "      \"fifty\": 1,\n"
        "      \"skip\": 1\n"
        "   }\n"
        "}\n\n", session_to_join->nb_lives);
    }

    send_response(cl, response);

    return 0;
}
