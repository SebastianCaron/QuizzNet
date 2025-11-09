#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "questions_generation.h"

void get_random_question_ids(server *s, int size, int* tab, int nb_themes, int* ids_themes){
    int i;
    SqliteResult* resquery;
    char query[512] = {'\0'};
    char number[64] = {'\0'};
    char *debut_query = "SELECT DISTINCT q.id FROM questions "
                        "NATURAL JOIN question_in_theme "
                        "NATURAL JOIN themes t "
                        "WHERE t.id IN (";
    strcat(query, debut_query);

    snprintf(number, sizeof(number), "%d", ids_themes[0]);
    strcat(query, number);

    for (i =1; i < nb_themes; i++){
        strcat(query, ",");
        snprintf(number, sizeof(number), "%d", ids_themes[i]);
        strcat(query, number);
    }

    strcat(query, ") ORDER BY RAND() LIMIT ");
    snprintf(number, sizeof(number), "%d", size);
    strcat(query, number);

    resquery = exec_query(s, query);

    for (i = 0; i < size; i++){
        tab[i] = atoi(resquery->rows[i][0]);
    }
}