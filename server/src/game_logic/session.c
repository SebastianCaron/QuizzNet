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

char has_everyone_answered(session *s){
    client *c;
    for(int i = 0; i < clist_size(s->players); i++){
        c = (client *)clist_get(s->players, i);
        if(c->infos_session.lives > 0 && c->infos_session.has_answered == 0) return 0;
    }
    return 1;
}

char is_everyone_dead(session *s){
    client *c;
    for(int i = 0; i < clist_size(s->players); i++){
        c = (client *)clist_get(s->players, i);
        if(c->infos_session.lives > 0) return 1;
    }
    return 0;
}

void send_session_start(session *s){
    char response[1024] = {'\0'};
    snprintf(response,sizeof(response), "POST session/started\n"
    "{"
    "   \"message\":\"session is starting\",\n"
    "   \"cooldown\": %d,\n"
    "}", SESSION_START_COOLDOWN);

    for(int i = 0; i < clist_size(s->players); i++){
        send_response((client *)clist_get(s->players, i), response);
    }
}

int *create_question_set(session *s){
    int *res = calloc(s->nb_questions, sizeof(int));
    if(!res){
        throw_error(MEMORY_ALLOCATION, "Erreur creation set de questions.");
        return res;
    }
    get_random_question_ids(s->server, s->nb_questions, res, s->nb_themes, s->themes_ids);

    return res;
}

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

    int *question_ids = create_question_set(_session);
    int nb_remaining_question = _session->nb_questions;

    send_session_start(_session);
    
    for (int i = 0; i < SESSION_START_COOLDOWN && !state->should_stop; i++) {
        sleep(1);
    }
    
    if (state->should_stop) {
        free(question_ids);
        session_destroy(_session);
        return NULL;
    }

    // BOUCLE DE JEU
    int current_question;
    int question_num = 0;
    while(!is_everyone_dead(_session) && nb_remaining_question > 0 && !state->should_stop){

        current_question = question_ids[_session->nb_questions - nb_remaining_question];
        get_question(_session->server, current_question, &q);
        
        _session->current_question = q;
        
        reset_session_players(_session);
        
        question_num = _session->nb_questions - nb_remaining_question + 1;
        send_session_question(_session, &q, question_num);

        time_t start_time = time(NULL);
        int time_limit_seconds = _session->time_limit;
        
        while(!has_everyone_answered(_session) && !state->should_stop) {
            time_t current_time = time(NULL);
            time_t elapsed_time = current_time - start_time;
            
            if(elapsed_time >= time_limit_seconds) {
                break;
            }
            
            for(int i = 0; i < clist_size(_session->players); i++){
                if(((client *)clist_get(_session->players, i))->infos_session.has_answered == 0) session_receive_for_player(_session, i);
            }
            
            usleep(2000);
        }

        if (state->should_stop) {
            break;
        }

        send_session_results(_session, question_num);

        nb_remaining_question--;

        for (int i = 0; i < cooldown_question && !state->should_stop; i++) {
            sleep(1);
        }
    }

    if (!state->should_stop) {
        send_session_finished(_session);
    }

    free(question_ids);
    session_destroy(_session);

    return NULL;
}

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

void session_receive_for_player(session *s, int i){

    client *p = clist_get(s->players, i);
    if(!p) return;
    int res = receive_from(p->fd, &s->buffer, &s->buffer_size, &s->buffer_capacity);

    if(res == -2){
        // Gerer erreur
    }

    handle_request_session(s, s->buffer, p);
}

void session_destroy(session *s){
    if(!s) return;

    free(s->name);
    free(s->themes_ids);

    int players_nb = clist_size(s->players);
    for(int i = 0; i < players_nb; i++){
        attach_client_to_server_procedure(s->server, clist_get(s->players, i));
    }
    clist_destroy(s->players);

    free(s);
}

// Searching for the session with the corresponding id
session* get_session_by_id(chained_list* l_sessions, int id_session){
    session* session_trouvee;
    for (int i = 0; i<clist_size(l_sessions); i++){
        if ((session_trouvee = clist_get(l_sessions, i))->id == id_session) return session_trouvee;
    }
    return NULL;
}