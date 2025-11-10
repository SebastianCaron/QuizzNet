#include <stdio.h>
#include <string.h>

#include "player_left.h"

#include "../endpoints.h"
#include "../../db/db.h"
#include "../../utils/chained_list.h"
#include "../../json/json.h"

int post_player_left(server *s, client *cl){
    infos_session_client* info_session = &cl->infos_session;
    session* session_actu = info_session->session;
    client* clt_tmp;
    int retour_snp;
    char response[1024] = {'\0'};
    char no_active_player = 1;
    int i;

    retour_snp = snprintf(response,1023,"{"
        "   \"pseudo\": %s,\n"
        "   \"reason\":\"disconnected\",\n"
        "}", cl->pseudo);

    if (retour_snp<0){
        throw_error(ENCODING_ERROR, "Erreur snprintf player left");
        return 1;
    }

    info_session->score = -1;

    //If disconnected client is creator, giving the creator status to another player
    if (info_session->is_creator == 1){
        info_session->is_creator = 0;
        for(i =0; i < session_actu->nb_players; i++){
            clt_tmp = clist_get(session_actu->players, i);
            if (clt_tmp->infos_session.score >= 0){
                clt_tmp->infos_session.is_creator = 1;
                break;
            }
        }
        if ((i+1) ==  session_actu->nb_players){
            session_destroy(session_actu);
            return 2;
        }
    }

    //sending response for each other player - if every player has disconnected, destroy session
    for(i = 0; i < session_actu->nb_players; i++){
        clt_tmp = clist_get(session_actu->players, i);
        if (clt_tmp->infos_session.score >= 0){
            send_response(clt_tmp, response);
            no_active_player = 0;
        }
    }
    if (no_active_player){
        session_destroy(session_actu);
        return 2;
    }

    return 0;

}