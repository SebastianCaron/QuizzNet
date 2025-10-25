#include <string.h>
#include "requestsdb.h"

char *get_statement(server *s, int id){
    int retour_snp;
    SqliteResult* resquery;
    char query[512];
    char* result = malloc(512*sizeof(char));
    if(!result){
        throw_error(MEMORY_ALLOCATION, "Erreur allocation resultat get_statement");
        return NULL;
    }

    retour_snp = snprintf(query, sizeof(query), "SELECT statement FROM questions q WHERE q.id = %d;", id);
    if (retour_snp<0){
        throw_error(ENCODING_ERROR, "Erreur snprintf create session CLASSIC");
        return NULL;
    }

    resquery = exec_query(s, query);
    strcpy(result, resquery->rows[0][0]);
    return result;
}

char *get_answers(server *s, int id){
    int retour_snp;
    SqliteResult* resquery;
    char query[512];
    char* result = malloc(512*sizeof(char));
    if(!result){
        throw_error(MEMORY_ALLOCATION, "Erreur allocation resultat get_answers");
        return NULL;
    }
    
    retour_snp = snprintf(query, sizeof(query), "SELECT answers FROM questions q WHERE q.id = %d;", id);
    if (retour_snp<0){
        throw_error(ENCODING_ERROR, "Erreur snprintf create session CLASSIC");
        return NULL;
    }

    resquery = exec_query(s, query);
    strcpy(result, resquery->rows[0][0]);
    return result;

}

int get_type(server *s, int id){
    SqliteResult* resquery;
    char query[512];
    int result;
    
    int retour_snp = snprintf(query, sizeof(query), "SELECT type FROM questions q WHERE q.id = %d;", id);
    if (retour_snp<0){
        throw_error(ENCODING_ERROR, "Erreur snprintf create session CLASSIC");
        return -1;
    }

    resquery = exec_query(s, query);
    result = atoi(resquery->rows[0][0]);
    return result;

}