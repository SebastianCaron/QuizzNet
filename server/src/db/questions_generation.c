#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "questions_generation.h"

void get_random_question_ids(server *s, int size, int* tab, int nb_themes, int* ids_themes, int nb_difficulties, int* difficulties){
    int i;
    SqliteResult* resquery;
    char query[512] = {'\0'};
    char number[64] = {'\0'};

    /* Build SELECT query with theme filter */
    char *debut_query = "SELECT DISTINCT q.id FROM questions "
                        "NATURAL JOIN question_in_theme "
                        "NATURAL JOIN themes t "
                        "WHERE t.id IN (";
    strcat(query, debut_query);

    /* Add first theme ID */
    snprintf(number, sizeof(number), "%d", ids_themes[0]);
    strcat(query, number);

    /* Add remaining theme IDs */
    for (i = 1; i < nb_themes; i++){
        strcat(query, ",");
        snprintf(number, sizeof(number), "%d", ids_themes[i]);
        strcat(query, number);
    }

    /* Add difficulty filter */
    strcat(query, ") AND q.difficulty IN (");
    
    snprintf(number, sizeof(number), "%d", difficulties[0]);
    strcat(query, number);
    
    for (i = 1; i < nb_difficulties; i++){
        strcat(query, ",");
        snprintf(number, sizeof(number), "%d", difficulties[i]);
        strcat(query, number);
    }

    /* Add random ordering and limit */
    strcat(query, ") ORDER BY RAND() LIMIT ");
    snprintf(number, sizeof(number), "%d", size);
    strcat(query, number);

    /* Execute query and extract IDs */
    resquery = exec_query(s, query);

    for (i = 0; i < size; i++){
        tab[i] = atoi(resquery->rows[i][0]);
    }
}
