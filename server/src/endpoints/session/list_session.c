#include <string.h>
#include <stdlib.h>
#include "list_session.h"
#include "../../db/db.h"
#include "../../network/network.h"
#include "../../utils/chained_list.h"
#include "../../game_logic/session.h"

char* get_theme_name(server *s, int theme_id) {
    char query[256];
    SqliteResult *result;
    char *theme_name = NULL;
    
    snprintf(query, sizeof(query), "SELECT name FROM themes WHERE id = %d;", theme_id);
    result = exec_query(s, query);
    
    if (result && result->row_count > 0) {
        theme_name = strdup(result->rows[0][0]);
    }
    
    if (result) {
        sqlite_result_destroy(result);
    }
    
    return theme_name;
}

int ensure_buffer_size(char **buffer, int *current_size, int needed_size) {
    if (needed_size >= *current_size) {
        int new_size = *current_size * 2;
        while (new_size < needed_size + 1024) {
            new_size *= 2;
        }
        
        char *new_buffer = realloc(*buffer, new_size);
        if (!new_buffer) {
            throw_error(MEMORY_ALLOCATION, "Erreur realloc buffer list session");
            return 1;
        }
        *buffer = new_buffer;
        *current_size = new_size;
    }
    return 0;
}

int get_session_list(server* s, char* request, client *cl){
    int buffer_size = 8192;
    char *response = calloc(buffer_size, sizeof(char));
    if(!response){
        throw_error(MEMORY_ALLOCATION, "Erreur allocation buffer list session");
        return 1;
    }
    
    int nb_session = clist_size(s->sessions);
    int retour_snp = snprintf(response, buffer_size - 1, "{"
        "   \"action\":\"sessions/list\",\n"
        "   \"statut\":\"200\",\n"
        "   \"message\":\"ok\",\n"
        "   \"nbSessions\": %d",
        nb_session);

    if (retour_snp<0){
        throw_error(ENCODING_ERROR, "Erreur snprintf list session");
        free(response);
        return 1;
    }

    int size = 0;
    int needed = 0;
    if(nb_session > 0){
        size = strlen(response);
        needed = size + 50;
        if (ensure_buffer_size(&response, &buffer_size, needed) != 0) {
            free(response);
            return 1;
        }
        
        retour_snp = snprintf(response + size, buffer_size - size - 1, ",\n   \"sessions\": [\n");
        if (retour_snp<0){
            throw_error(ENCODING_ERROR, "Erreur snprintf list session");
            free(response);
            return 1;
        }
        
        for(int i = 0; i < nb_session; i++){
            session *sess = (session *)clist_get(s->sessions, i);
            if (!sess) continue;
            
            size = strlen(response);
            char session_json[4096] = {'\0'};
            
            retour_snp = snprintf(session_json, sizeof(session_json), "{\n"
                "   \"id\": %d,\n"
                "   \"name\": \"%s\",\n",
                sess->id, sess->name ? sess->name : "");
            
            if (retour_snp < 0) {
                throw_error(ENCODING_ERROR, "Erreur snprintf session");
                free(response);
                return 1;
            }
            
            int len = strlen(session_json);
            
            len += snprintf(session_json + len, sizeof(session_json) - len, "   \"themeIds\": [");
            
            int nb_themes = 0;
            if (sess->themes_ids) {
                while (sess->themes_ids[nb_themes] > 0 && nb_themes < 100) {
                    nb_themes++;
                }
            }
            
            for (int j = 0; j < nb_themes; j++) {
                len += snprintf(session_json + len, sizeof(session_json) - len, "%d", sess->themes_ids[j]);
                if (j < nb_themes - 1) {
                    len += snprintf(session_json + len, sizeof(session_json) - len, ",");
                }
            }
            
            len += snprintf(session_json + len, sizeof(session_json) - len, "],\n");
            
            len += snprintf(session_json + len, sizeof(session_json) - len, "   \"themeNames\": [");
            
            for (int j = 0; j < nb_themes; j++) {
                char *theme_name = get_theme_name(s, sess->themes_ids[j]);
                if (theme_name) {
                    len += snprintf(session_json + len, sizeof(session_json) - len, "\"%s\"", theme_name);
                    free(theme_name);
                } else {
                    len += snprintf(session_json + len, sizeof(session_json) - len, "\"\"");
                }
                if (j < nb_themes - 1) {
                    len += snprintf(session_json + len, sizeof(session_json) - len, ",");
                }
            }
            
            len += snprintf(session_json + len, sizeof(session_json) - len, "],\n");
            
            len += snprintf(session_json + len, sizeof(session_json) - len,
                "   \"difficulty\":\"%s\",\n"
                "   \"nbQuestions\":%d,\n"
                "   \"timeLimit\":%d,\n"
                "   \"mode\":\"%s\",\n"
                "   \"nbPlayers\":%d,\n"
                "   \"maxPlayers\":%d,\n"
                "   \"status\":\"%s\"\n"
                "}",
                difficulty_to_string(sess->difficulty),
                sess->nb_questions,
                sess->time_limit,
                session_type_to_string(sess->type),
                sess->nb_players,
                sess->max_nb_players,
                session_status_to_string(sess->status));
            
            if (len < 0 || len >= (int)sizeof(session_json)) {
                throw_error(ENCODING_ERROR, "Erreur snprintf session JSON");
                free(response);
                return 1;
            }
            
            size = strlen(response);
            int remaining = buffer_size - size - 1;
            
            if (remaining < len + 10) {
                int needed = size + len + 1024;
                if (ensure_buffer_size(&response, &buffer_size, needed) != 0) {
                    free(response);
                    return 1;
                }
                remaining = buffer_size - size - 1;
            }
            
            retour_snp = snprintf(response + size, remaining, "%s", session_json);
            
            if (retour_snp < 0 || retour_snp >= remaining) {
                throw_error(ENCODING_ERROR, "Erreur snprintf ajout session");
                free(response);
                return 1;
            }
            
            if (i < nb_session - 1) {
                size = strlen(response);
                if (size < buffer_size - 2) {
                    response[size] = ',';
                    response[size + 1] = '\n';
                    response[size + 2] = '\0';
                } else {
                    int needed = size + 10;
                    if (ensure_buffer_size(&response, &buffer_size, needed) != 0) {
                        free(response);
                        return 1;
                    }
                    response[size] = ',';
                    response[size + 1] = '\n';
                    response[size + 2] = '\0';
                }
            }
        }
        
        size = strlen(response);
        needed = size + 50;
        if (ensure_buffer_size(&response, &buffer_size, needed) != 0) {
            free(response);
            return 1;
        }
        
        retour_snp = snprintf(response + size, buffer_size - size - 1, "\n   ]");
        if (retour_snp < 0) {
            throw_error(ENCODING_ERROR, "Erreur snprintf fermeture tableau");
            free(response);
            return 1;
        }
    }
    
    size = strlen(response);
    needed = size + 10;
    if (ensure_buffer_size(&response, &buffer_size, needed) != 0) {
        free(response);
        return 1;
    }
    
    snprintf(response + size, buffer_size - size - 1, "\n}");
    
    send_response(cl, response);
    free(response);
    
    return 0;
}