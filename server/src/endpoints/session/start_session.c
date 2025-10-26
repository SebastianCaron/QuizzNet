#include <pthread.h>

#include "start_session.h"

int post_session_start(server* s, char* request, client *cl){
    
    if(!cl->infos_session.is_creator){
        char *response =
        "{"
        "   \"action\":\"session/start\",\n"
        "   \"statut\":\"404\",\n"
        "   \"message\":\"there is no session that you can start\"\n"
        "}";
        send_response(cl, response);
        return 1;
    }
    int i = 0;
    session *_session = cl->infos_session.session;
    
    if(_session->nb_players < 2){
        char *response =
        "{"
        "   \"action\":\"session/start\",\n"
        "   \"statut\":\"201\",\n"
        "   \"message\":\"not enough player in session\"\n"
        "}";
        send_response(cl, response);
        return 1;
    }

    int players_size = clist_size(_session->players);
    for(i = 0; i < players_size; i++){
        remove_client_from_server_procedure(s, clist_get(_session->players, i)); // PEUT ETRE OPTI
    }

    pthread_t udp_thread;
    if (pthread_create(&udp_thread, NULL, handle_session, _session) != 0) {
        throw_error(THREAD_CREATION, "Erreur creation du thread.");
        for(i = 0; i < players_size; i++){
            attach_client_to_server_procedure(s, clist_get(_session->players, i));
        }

        char *response =
        "{"
        "   \"action\":\"session/start\",\n"
        "   \"statut\":\"500\",\n"
        "   \"message\":\"Error when starting the session\"\n"
        "}";
        send_response(cl, response);
        return 1;
    }

    return 0;
}