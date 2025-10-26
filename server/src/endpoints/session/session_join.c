#include <string.h>
#include "session_join.h"
#include "../../game_logic/session.h"
#include "../endpoints.h"
#include "../../network/network.h"
#include "../../json/json.h"
#include "../../network/client.h"
#include "../../utils/chained_list.h"

int post_session_join(server* s, char* request, client *cl){
    int retour_snp;
    int player_size;
    session* session_to_join;
    char* str_tmp;
    client* player;
    char response[1024] = {'\0'};
    char *response_other_players = {'\0'};
    cJSON *json = cJSON_Parse(request);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            printf("Error: %s\n", error_ptr);
        }
        cJSON_Delete(json);
        return 1;
    }

    session_to_join = get_session_by_id(s->sessions, get_from_json_int(json, "sessionId"));
    if (session_to_join->nb_players == session_to_join->max_nb_players){
            char *response_full =
            "{"
            "   \"action\":\"session/join\",\n"
            "   \"statut\":\"403\",\n"
            "   \"message\":\"session is full\"\n"
            "}";
            send_response(cl, response_full);
            return 0;
    }
    clist_append(session_to_join->players, cl);
    session_to_join->nb_players++;
    retour_snp = snprintf(response_other_players,1023,"{"
            "   \"pseudo\":\"%s\",\n"
            "   \"nbPlayers\":\"%d\",\n"
            "}", cl->pseudo , session_to_join->nb_players);

    if (retour_snp<0){
        throw_error(ENCODING_ERROR, "Erreur snprintf join session response other players");
        return 1;
    }


    // TODO Ajouter au client la session auquel il est connecté
    // NON CREATEUR. A voir si un créareur peut join sa propre session
    // SI OUI, il faudra changer la réponse (ici creator tjrs false)

    player_size = clist_size(session_to_join->players);

    if (session_to_join->type == CLASSIC){
        retour_snp = snprintf(response,1023,"{"
        "   \"action\":\"session/join\",\n"
        "   \"statut\":\"201\",\n"
        "   \"message\":\"session joined\",\n"
        "   \"sessionId\": %d,\n"
        "   \"mode\":\"solo\",\n"
        "   \"isCreator\": false,\n"
        "   \"players\": [", session_to_join->id);
        
        if (retour_snp<0){
            throw_error(ENCODING_ERROR, "Erreur snprintf join session CLASSIC");
            return 1;
        }

        for (int i = 0; i<player_size; i++){
            player = ((client *) (clist_get(session_to_join->players, i)));
            str_tmp = player->pseudo;
            retour_snp = snprintf(response + strlen(response), sizeof(response) - strlen(response),"\"%s\"", str_tmp);
            if (retour_snp<0){
                throw_error(ENCODING_ERROR, "Erreur snprintf join session CLASSIC");
                return 1;
            }

            if((i+1)!=player_size) strcat(response, ",");
            send_response(player, response_other_players);
        }
        strcat(response, "],\n"
        "   \"jokers\":{\n"
        "      \"fifty\": 1,\n"
        "      \"skip\": 1\n"
        "   }"
        "}");

    } else {
         retour_snp = snprintf(response,1023,"{"
        "   \"action\":\"session/join\",\n"
        "   \"statut\":\"201\",\n"
        "   \"message\":\"session joined\",\n"
        "   \"sessionId\": %d,\n"
        "   \"mode\":\"battle\",\n"
        "   \"isCreator\": false,\n"
        "   \"players\": [", session_to_join->id);
        
        if (retour_snp<0){
            throw_error(ENCODING_ERROR, "Erreur snprintf join session BATTLE");
            return 1;
        }

        for (int i = 0; i<player_size; i++){
            player = ((client *) (clist_get(session_to_join->players, i)));
            str_tmp = player->pseudo;
            retour_snp = snprintf(response + strlen(response), sizeof(response) - strlen(response),"\"%s\"", str_tmp);
            if (retour_snp<0){
                throw_error(ENCODING_ERROR, "Erreur snprintf join session BATTLE");
                return 1;
        }

            if((i+1)!=player_size) strcat(response, ",");
            send_response(player, response_other_players);
        }

        snprintf(response + strlen(response), sizeof(response) - strlen(response), "],\n"
        "   \"lives\": %d,\n"
        "   \"jokers\":{\n"
        "      \"fifty\": 1,\n"
        "      \"skip\": 1\n"
        "   }"
        "}", session_to_join->nb_lives);
    }

    send_response(cl, response);

    return 0;
}