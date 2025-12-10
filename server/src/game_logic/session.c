#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "session.h"
#include "../endpoints/endpoints.h"
#include "../utils/chained_list.h"
#include "../db/questions_generation.h"
#include "../db/requestsdb.h"
#include "session_responses/session_question.h"
#include "session_responses/session_results.h"
#include "session_responses/session_finished.h"
#include "../endpoints/game/joker_use.h"
#include "../endpoints/game/question_answer.h"
#include "../network/server.h"

session_type get_session_type(char* mode){
    return strcmp("solo", mode) ? BATTLE : CLASSIC;
}

difficulty get_session_difficulty(char* difficulty_str){
    if (!(strcmp("moyen", difficulty_str))) return MEDIUM;
    if (!(strcmp("difficile", difficulty_str))) return HARD;
    return EASY;
}

const char* difficulty_to_string(difficulty d) {
    switch(d) {
        case EASY: return "facile";
        case MEDIUM: return "moyen";
        case HARD: return "difficile";
        default: return "facile";
    }
}

const char* session_type_to_string(session_type t) {
    switch(t) {
        case CLASSIC: return "solo";
        case BATTLE: return "battle";
        default: return "solo";
    }
}

const char* session_status_to_string(session_status s) {
    switch(s) {
        case WAITING: return "waiting";
        case PLAYING: return "playing";
        case FINISHED: return "finished";
        default: return "waiting";
    }
}

/**
 * @brief Checks if all active players have answered the current question.
 * 
 * @param s Pointer to the session.
 * @return 1 if everyone answered, 0 otherwise.
 */
char has_everyone_answered(session *s){
    client *c;
    for(int i = 0; i < clist_size(s->players); i++){
        c = (client *)clist_get(s->players, i);
        if(c->infos_session.lives > 0 && c->infos_session.has_answered == 0) return 0;
    }
    return 1;
}

/**
 * @brief Checks if all players are dead (no lives remaining).
 * 
 * @param s Pointer to the session.
 * @return 1 if everyone is dead, 0 if at least one player is alive.
 */
char is_everyone_dead(session *s){
    client *c;
    for(int i = 0; i < clist_size(s->players); i++){
        c = (client *)clist_get(s->players, i);
        if(c->infos_session.lives > 0) return 0;  /* Someone is still alive */
    }
    return 1;  /* Everyone is dead */
}

/**
 * @brief Sends the session start notification to all players.
 * 
 * @param s Pointer to the session.
 */
void send_session_start(session *s){
    char response[1024] = {'\0'};
    snprintf(response, sizeof(response), "POST session/started\n"
    "{"
    "   \"message\":\"session is starting\",\n"
    "   \"cooldown\": %d\n"
    "}\n\n", SESSION_START_COOLDOWN);

    for(int i = 0; i < clist_size(s->players); i++){
        send_response((client *)clist_get(s->players, i), response);
    }
}

/**
 * @brief Creates a set of random question IDs for the session.
 * 
 * @param s Pointer to the session.
 * @return Array of question IDs, or NULL on error.
 */
int *create_question_set(session *s){
    int *res = calloc(s->nb_questions, sizeof(int));
    if(!res){
        throw_error(MEMORY_ALLOCATION, "Erreur creation set de questions.");
        return res;
    }
    
    /* Convert session difficulty to database format */
    int db_difficulty = (int)s->difficulty + 1;
    int difficulties[1] = {db_difficulty};
    
    get_random_question_ids(s->server, s->nb_questions, res, s->nb_themes, s->themes_ids, 1, difficulties);

    return res;
}

/**
 * @brief Resets player answer states for a new question.
 * 
 * @param s Pointer to the session.
 */
void reset_session_players(session *s){
    for(int i = 0; i < clist_size(s->players); i++){
        client *c = (client *)clist_get(s->players, i);
        if(c) {
            c->infos_session.has_answered = 0;
            c->infos_session.skip = 0;
        }
    }
}

void *handle_session(void *args){
    session *_session = (session *) args;
    int cooldown_question = SESSION_QUESTION_COOLDOWN;
    server_state *state = get_server_state();

    question q;

    /* Generate question set for the session */
    int *question_ids = create_question_set(_session);
    int nb_remaining_question = _session->nb_questions;

    /* Send start notification and wait for cooldown */
    send_session_start(_session);
    
    for (int i = 0; i < SESSION_START_COOLDOWN && !state->should_stop; i++) {
        sleep(1);
    }
    
    /* Check for server shutdown */
    if (state->should_stop) {
        free(question_ids);
        session_destroy(_session);
        return NULL;
    }

    /* Main game loop */
    int current_question;
    int question_num = 0;
    while(!is_everyone_dead(_session) && nb_remaining_question > 0 && !state->should_stop){

        /* Get current question from database */
        current_question = question_ids[_session->nb_questions - nb_remaining_question];
        get_question(_session->server, current_question, &q);
        
        _session->current_question = q;
        
        /* Reset player states for new question */
        reset_session_players(_session);
        
        /* Send question to all players */
        question_num = _session->nb_questions - nb_remaining_question + 1;
        send_session_question(_session, &q, question_num);

        /* Wait for answers with time limit */
        time_t start_time = time(NULL);
        int time_limit_seconds = _session->time_limit;
        
        while(!has_everyone_answered(_session) && !state->should_stop) {
            time_t current_time = time(NULL);
            time_t elapsed_time = current_time - start_time;
            
            /* Time limit reached */
            if(elapsed_time >= time_limit_seconds) {
                break;
            }
            
            /* Poll for player responses */
            for(int i = clist_size(_session->players) - 1; i >= 0; i--){
                client *p = (client *)clist_get(_session->players, i);
                if(p && p->infos_session.has_answered == 0) {
                    session_receive_for_player(_session, i);
                }
            }
            
            usleep(2000);  /* Small delay to avoid busy waiting */
        }

        if (state->should_stop) {
            break;
        }

        /* Calculate and send results */
        send_session_results(_session, question_num);

        nb_remaining_question--;

        /* Cooldown between questions */
        for (int i = 0; i < cooldown_question && !state->should_stop; i++) {
            sleep(1);
        }
    }

    /* Send final results if game completed normally */
    if (!state->should_stop) {
        send_session_finished(_session);
    }

    free(question_ids);
    session_destroy(_session);

    return NULL;
}

/**
 * @brief Handles in-game requests from a player.
 * 
 * Routes joker usage and answer submissions to appropriate handlers.
 * 
 * @param s Pointer to the session.
 * @param request The request string.
 * @param p Pointer to the player client.
 */
void handle_request_session(session *s, char *request, client *p){
    endpoints ep = get_endpoint(request);
    switch (ep)
    {

    case POST_JOKER_USE:
        post_joker_use(s, request, p);
        break;
    case POST_QUESTION_ANSWER:
        post_question_answer(s, request, p);
        break;

    /* These endpoints are not valid during a game */
    case POST_PLAYER_REGISTER:
    case POST_PLAYER_LOGIN:
    case POST_SESSION_CREATE:
    case GET_THEMES_LIST:
    case GET_SESSION_LIST:
    case INVALID_ENDPOINT:
        send_invalid_response(p);
        break;
    
    default:
        send_error_response(p);
        break;
    }
}

void session_remove_client(session *s, client *cl){
    if(!s || !cl) return;
    
    int client_index = clist_find(s->players, cl);
    if(client_index == -1) return;
    
    /* Transfer creator role if creator is leaving */
    if(cl->infos_session.is_creator && clist_size(s->players) > 1){
        for(int i = 0; i < clist_size(s->players); i++){
            client *other_client = (client *)clist_get(s->players, i);
            if(other_client != cl && other_client){
                other_client->infos_session.is_creator = 1;
                break;
            }
        }
    }
    
    /* Remove from session */
    clist_remove(s->players, cl);
    s->nb_players--;
    
    /* Clear client session info */
    cl->infos_session.session = NULL;
    cl->infos_session.is_creator = 0;
    
    /* Destroy empty sessions that aren't playing */
    if(clist_size(s->players) == 0 && s->status != PLAYING){
        clist_remove(s->server->sessions, s);
        session_destroy(s);
    }
}

void session_receive_for_player(session *s, int i){
    if(!s) return;
    
    client *p = clist_get(s->players, i);
    if(!p) return;

    int res = receive_from(p->fd, &p->buffer_cl);

    /* Handle disconnection */
    if(res == -1){
        session_remove_client(s, p);
        client_destroy(p);
        return;
    }
    
    /* Handle error */
    if(res == -2){
        session_remove_client(s, p);
        client_destroy(p);
        return;
    }

    /* Process complete requests */
    if(res > 0 && request_available(&p->buffer_cl)){
        char *request = get_request(&p->buffer_cl);
        if(request){
            handle_request_session(s, request, p);
            update_buffer(&p->buffer_cl, strlen(request));
            free(request);
        }
    }
    reset_full_buffer(&p->buffer_cl);
}

void session_destroy(session *s){
    if(!s) return;

    free(s->name);
    free(s->themes_ids);

    /* Re-attach all players to main server loop */
    int players_nb = clist_size(s->players);
    for(int i = 0; i < players_nb; i++){
        attach_client_to_server_procedure(s->server, clist_get(s->players, i));
    }
    clist_destroy(s->players);

    free(s);
}

session* get_session_by_id(chained_list* l_sessions, int id_session){
    session* session_trouvee;
    for (int i = 0; i < clist_size(l_sessions); i++){
        if ((session_trouvee = clist_get(l_sessions, i))->id == id_session) return session_trouvee;
    }
    return NULL;
}
