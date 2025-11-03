#include <stdio.h>
#include <string.h>

#include "get_themes_list.h"

#include "../endpoints.h"
#include "../../db/db.h"
#include "../../json/json.h"

int get_themes_list(server *s, char *request, client *cl){
    char response[1024] = {'\0'};
    SqliteResult *res = exec_query(s, "SELECT id, name FROM themes");

    retour_snp = snprintf(response,1023,"{"
    "   \"action\":\"themes/list\",\n"
    "   \"statut\":\"200\",\n"
    "   \"message\":\"ok\",\n"
    "   \"nbThemes\": %d,\n"
    "   \"themes\": [", res->row_count);
        
    if (retour_snp<0){
        throw_error(ENCODING_ERROR, "Erreur snprintf get themes list");
        return 1;
    }

    for (int i =0; i< res->row_count : i++){
        retour_snp = snprintf(response + strlen(response), sizeof(response) - strlen(response),
    "       {"
    "                \"id\":%d,"
    "                \"name\":\"%s\""
    "        },\n", res->rows[i][0], res->rows[i][1]);

        if (retour_snp<0){
            throw_error(ENCODING_ERROR, "Erreur snprintf get themes list in boucle");
            return 1;
        }
    }

    strcat(response, "  ],\n"
    "}");
    
    send_response(cl, response);
    return 0;
}