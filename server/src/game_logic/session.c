#include <string.h>
#include "session.h"

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