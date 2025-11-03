#include <string.h>
#include <stdlib.h>

#include "session.h"
#include "../endpoints/endpoints.h"
#include "../utils/chained_list.h"

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


void *handle_session(void *args){
    session *_session = (session *) args;
    const char response[1024] = {'\0'};
    int cooldown = SESSION_START_COOLDOWN;

    snprintf(response,sizeof(response), "POST session/started\n"
    "{"
    "   \"message\":\"session is starting\",\n"
    "   \"cooldown\": %d,\n"
    "}", cooldown);

    // TODO: GERER LES ERREURS
    // TODO: GERER LES DECONNEXIONS CLIENTS

    for(int i = 0; i < clist_size(_session->players); i++){
        send_response((client *)clist_get(_session->players, i), response);
    }

    sleep(cooldown);

    // BOUCLE DE JEU
    // TABLEAU DE BOOL JOUEUR A REPONDU ? []
    // POUR LE NOMRE DE QUESTION DU SET 
        // ENVOYER LA QUESTION A TOUS LES JOUEURS
        // TANT QUIL RESTE DU TEMPS OU QUIL RESTE DES GENS QUI NONT PAS REPS : 
            // RECUPERER REQUETES/REPONSES DES JOUEURS => HANDLE
        // AFFICHER RESULTATS DE LA QUESTION

        // POTENTIELLEMENT ELIMINER LES JOUEURS
    
    // AFFICHER SESSION FINISHED


    // REATACHER LES CLIENTS AU PROCESS PRINCIPAL

    // DETRUIRE SESSION

    return NULL;
}

void session_receive_for_player(session *s, int i){

    client *p = clist_get(s->players, i);
    if(!p) return;
    int res = receive_from(p->fd, &s->buffer, &s->buffer_size, &s->buffer_capacity);

    if(res == -2){
        // Gerer erreur
    }

    // handle_request_session(s, s->buffer, p);
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