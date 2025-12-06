#include <string.h>
#include <stdlib.h>

#include "session_finished.h"
#include "../../endpoints/endpoints.h"
#include "../../utils/chained_list.h"
#include "../../json/cJSON.h"
#include "../session.h"
#include "../../errors/error.h"

/**
 * @brief Comparison function for CLASSIC mode ranking.
 * 
 * Sorts by score (descending), then by correct answers (descending).
 */
static int compare_solo(const void *a, const void *b) {
    client *client_a = *(client **)a;
    client *client_b = *(client **)b;
    
    /* Primary: higher score first */
    if(client_b->infos_session.score != client_a->infos_session.score) {
        return client_b->infos_session.score - client_a->infos_session.score;
    }
    
    /* Secondary: more correct answers first */
    return client_b->infos_session.correctAnswers - client_a->infos_session.correctAnswers;
}

/**
 * @brief Comparison function for BATTLE mode ranking.
 * 
 * Sorts by lives (descending), then by elimination order (later is better),
 * then by score (descending).
 */
static int compare_battle(const void *a, const void *b) {
    client *client_a = *(client **)a;
    client *client_b = *(client **)b;
    
    /* Primary: more lives first */
    if(client_b->infos_session.lives != client_a->infos_session.lives) {
        return client_b->infos_session.lives - client_a->infos_session.lives;
    }
    
    /* Survivors (eliminatedAt == 0) rank higher than eliminated */
    if(client_a->infos_session.eliminatedAt == 0 && client_b->infos_session.eliminatedAt != 0) {
        return -1;
    }
    if(client_a->infos_session.eliminatedAt != 0 && client_b->infos_session.eliminatedAt == 0) {
        return 1;
    }
    
    /* Later elimination is better */
    if(client_a->infos_session.eliminatedAt != 0 && client_b->infos_session.eliminatedAt != 0) {
        return client_b->infos_session.eliminatedAt - client_a->infos_session.eliminatedAt;
    }
    
    /* Tertiary: higher score first */
    return client_b->infos_session.score - client_a->infos_session.score;
}

void send_session_finished(session *s){
    if(!s) return;
    
    int nb_players = clist_size(s->players);

    /* Create array for sorting */
    client **players_array = malloc(nb_players * sizeof(client *));
    if(!players_array) {
        throw_error(MEMORY_ALLOCATION, "Erreur allocation pour players_array dans send_session_finished");
        return;
    }
    
    for(int i = 0; i < nb_players; i++) {
        players_array[i] = (client *)clist_get(s->players, i);
    }
    
    /* Sort players based on game mode */
    if(s->type == CLASSIC) {
        qsort(players_array, nb_players, sizeof(client *), compare_solo);
    } else {
        qsort(players_array, nb_players, sizeof(client *), compare_battle);
    }
    
    /* Build response JSON */
    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddStringToObject(response_json, "mode", session_type_to_string(s->type));
    
    /* Add winner for BATTLE mode */
    if(s->type == BATTLE && nb_players > 0 && players_array[0]) {
        cJSON_AddStringToObject(response_json, "winner", players_array[0]->pseudo ? players_array[0]->pseudo : "");
    }
    
    /* Build ranking array */
    cJSON *ranking_array = cJSON_CreateArray();
    
    for(int i = 0; i < nb_players; i++) {
        client *c = players_array[i];
        if(!c) continue;
        
        cJSON *player_rank = cJSON_CreateObject();
        cJSON_AddNumberToObject(player_rank, "rank", i + 1);
        cJSON_AddStringToObject(player_rank, "pseudo", c->pseudo ? c->pseudo : "");
        
        if(s->type == CLASSIC) {
            cJSON_AddNumberToObject(player_rank, "score", c->infos_session.score);
            cJSON_AddNumberToObject(player_rank, "correctAnswers", c->infos_session.correctAnswers);
        } else {
            cJSON_AddNumberToObject(player_rank, "lives", c->infos_session.lives);
            cJSON_AddNumberToObject(player_rank, "score", c->infos_session.score);
            cJSON_AddNumberToObject(player_rank, "correctAnswers", c->infos_session.correctAnswers);
            
            if(c->infos_session.eliminatedAt > 0) {
                cJSON_AddNumberToObject(player_rank, "eliminatedAt", c->infos_session.eliminatedAt);
            }
        }
        
        cJSON_AddItemToArray(ranking_array, player_rank);
    }
    
    cJSON_AddItemToObject(response_json, "ranking", ranking_array);
    
    /* Send to all players */
    char *json_string = cJSON_Print(response_json);
    if(json_string) {
        char response[4096] = {'\0'};
        snprintf(response, sizeof(response), "POST session/finished\n%s", json_string);
        
        for(int i = 0; i < nb_players; i++) {
            client *c = players_array[i];
            if(c) {
                send_response(c, response);
            }
        }
        
        free(json_string);
    }
    
    cJSON_Delete(response_json);
    free(players_array);
}
