#include "questions_generation.h"

void get_questions(int size, int* tab, int nb_themes, int* ids_themes){
    int i;
    SqliteResult* resquery;
    char query = malloc(512*sizeof(char));
    char* debut_query = "SELECT DISTINCT q.id FROM questions "
                        "NATURAL JOIN question_in_theme "
                        "NATURAL JOIN themes t "
                        "WHERE t.id IN (";

    strcat(query, itoa(ids_themes[0]));

    for (i =1; i < nb_themes; i++){
        query = strcat(query, ",")
        query = strcat(itoa(ids_themes[i]))
    }

    strcat(query, ") ORDER BY RAND() LIMIT ");
    strcat(query, itoa(size));

    resquery = exec_query(s, query);

    for (i = 0; i < size; i++){
        tab[i] = atoi(resquery->row[i][0])
    }
}