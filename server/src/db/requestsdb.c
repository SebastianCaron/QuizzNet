#include <string.h>
#include <stdlib.h>
#include "requestsdb.h"
#include "../errors/error.h"
#include "../game_logic/session_objects.h"
#include "../json/cJSON.h"

difficulty int_to_difficulty(int diff_int) {
    switch(diff_int) {
        case 0: return EASY;
        case 1: return MEDIUM;
        case 2: return HARD;
        default: return EASY;
    }
}

question_type int_to_question_type(int type_int) {
    switch(type_int) {
        case 0: return QCM;
        case 1: return TRUEFALSE;
        case 2: return FREETEXT;
        default: return QCM;
    }
}

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

int get_question(server *s, int id, question *q){
    if(!s || !q) {
        throw_error(NULL_VARIABLE, "get_question: paramètres NULL");
        return -1;
    }
    
    SqliteResult* resquery;
    char query[512];
    int retour_snp;
    
    retour_snp = snprintf(query, sizeof(query), 
                          "SELECT id, difficulty, type, statement, answers FROM questions WHERE id = %d;", id);
    if(retour_snp < 0) {
        throw_error(ENCODING_ERROR, "Erreur snprintf dans get_question");
        return -1;
    }
    
    resquery = exec_query(s, query);
    if(!resquery) {
        throw_error(DATABASE, "Erreur lors de l'exécution de la requête dans get_question");
        return -1;
    }
    
    if(resquery->row_count == 0) {
        sqlite_result_destroy(resquery);
        throw_error(NOT_FOUND, "Question non trouvée dans get_question");
        return -1;
    }
    
    q->id = atoi(resquery->rows[0][0]);
    q->diff = int_to_difficulty(atoi(resquery->rows[0][1]));
    q->type = int_to_question_type(atoi(resquery->rows[0][2]));
    
    strncpy(q->statement, resquery->rows[0][3] ? resquery->rows[0][3] : "", sizeof(q->statement) - 1);
    q->statement[sizeof(q->statement) - 1] = '\0';

    strncpy(q->answer, resquery->rows[0][4] ? resquery->rows[0][4] : "", sizeof(q->answer) - 1);
    q->answer[sizeof(q->answer) - 1] = '\0';
    
    sqlite_result_destroy(resquery);
    return 0;
}