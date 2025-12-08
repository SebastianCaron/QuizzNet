#include <stdio.h>
#include <string.h>

#include "get_themes_list.h"

#include "../endpoints.h"
#include "../../db/db.h"
#include "../../json/json.h"

int get_themes_list(server *s, char *request, client *cl){
    char response[1024] = {'\0'};

    /* Query all themes from database */
    SqliteResult *res = exec_query(s, "SELECT id, name FROM themes;");

    /* Build JSON response header */
    int retour_snp = snprintf(response, 1023, "{"
    "   \"action\":\"themes/list\",\n"
    "   \"statut\":\"200\",\n"
    "   \"message\":\"ok\",\n"
    "   \"nbThemes\": %d,\n"
    "   \"themes\": [", res->row_count);
        
    if (retour_snp < 0){
        throw_error(ENCODING_ERROR, "Erreur snprintf get themes list");
        return 1;
    }

    /* Add each theme to the array */
    for (int i = 0; i < res->row_count; i++){
        if (i < res->row_count - 1) {
            retour_snp = snprintf(response + strlen(response), sizeof(response) - strlen(response),
        "       {"
        "                \"id\":%d,"
        "                \"name\":\"%s\""
        "        },\n", atoi(res->rows[i][0]), res->rows[i][1]);
        } else {
            /* Last element without trailing comma */
            retour_snp = snprintf(response + strlen(response), sizeof(response) - strlen(response),
        "       {"
        "                \"id\":%d,"
        "                \"name\":\"%s\""
        "        }\n", atoi(res->rows[i][0]), res->rows[i][1]);
        }

        if (retour_snp < 0){
            throw_error(ENCODING_ERROR, "Erreur snprintf get themes list in boucle");
            return 1;
        }
    }

    /* Close JSON array and object */
    strcat(response, "  ]\n"
    "}\n\n");
    
    send_response(cl, response);
    return 0;
}
