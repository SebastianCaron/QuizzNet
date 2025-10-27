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

    // TODO: TRAITER LES REQUETES DES PLAYERS
        //__ > TODO: CREER UN BUFFER POUR LA SESSION | PEUT ETRE REFACTOR NOTRE MANIERE DE RECUP LES REQUETES

    return NULL;
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