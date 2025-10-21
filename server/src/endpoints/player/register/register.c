#include "../../../network/client.h"
#include "../../../network/network.h"
#include "../../endpoints.h"
#include "../../../db/db.h"
#include <stdio.h>

int post_player_register(server *s, char *requets, client *cl){
    char *request[1024] = {'\0'};
    char *base = "";
    snprintf(request, 1023, "INSERT INTO clients(pseudo, password) VALUES (%s, %s);", 
        get_from_json(request, "pseudo"), get_from_json(request, "password"));
    
    SqliteResult *res = exec_query(s, request);
    
    if(!res){
        // RETOURNER ERREUR
        return 1;
    }

    

}