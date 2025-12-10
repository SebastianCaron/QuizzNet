#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "session_question.h"
#include "../../endpoints/endpoints.h"
#include "../../utils/chained_list.h"
#include "../../json/cJSON.h"
#include "../session.h"

/**
 * @brief Creates a shuffled copy of a cJSON string array.
 * 
 * @param array Original cJSON array.
 * @return New shuffled cJSON array, or NULL on error.
 */
cJSON *create_shuffled_array(cJSON *array) {
    if(!array || !cJSON_IsArray(array)) return NULL;
    
    int size = cJSON_GetArraySize(array);
    if(size == 0) return cJSON_CreateArray();
    
    /* Copy strings to temporary array */
    char **strings = malloc(size * sizeof(char *));
    if(!strings) return NULL;
    
    for(int i = 0; i < size; i++) {
        cJSON *item = cJSON_GetArrayItem(array, i);
        if(item && cJSON_IsString(item)) {
            strings[i] = strdup(item->valuestring);
        } else {
            strings[i] = strdup("");
        }
    }
    
    /* Fisher-Yates shuffle */
    for(int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        if(i != j) {
            char *temp = strings[i];
            strings[i] = strings[j];
            strings[j] = temp;
        }
    }
    
    /* Create new array with shuffled strings */
    cJSON *shuffled = cJSON_CreateArray();
    for(int i = 0; i < size; i++) {
        cJSON_AddItemToArray(shuffled, cJSON_CreateString(strings[i]));
        free(strings[i]);
    }
    free(strings);
    
    return shuffled;
}

void send_session_question(session *s, question *q, int question_num){
    if(!s || !q) return;

    /* Build question JSON */
    cJSON *json = cJSON_CreateObject();
    if(!json) return;

    cJSON_AddNumberToObject(json, "questionNum", question_num);
    cJSON_AddNumberToObject(json, "totalQuestions", s->nb_questions);
    cJSON_AddStringToObject(json, "difficulty", difficulty_to_string(q->diff));
    cJSON_AddStringToObject(json, "question", q->statement);

    const char *type_str;

    if(q->type == QCM) {
        type_str = "qcm";
        cJSON_AddStringToObject(json, "type", type_str);
        
        /* Parse answers */
        cJSON *answers_json = cJSON_Parse(q->answer);
        if(answers_json && cJSON_IsArray(answers_json)) {
            /* Remember correct answer before shuffling (first answer is correct) */
            cJSON *correct_answer_item = cJSON_GetArrayItem(answers_json, 0);
            char *correct_answer = NULL;
            if(correct_answer_item && cJSON_IsString(correct_answer_item)) {
                correct_answer = strdup(correct_answer_item->valuestring);
            }
            
            /* Create shuffled copy of answers */
            cJSON *shuffled_answers = create_shuffled_array(answers_json);
            cJSON_Delete(answers_json);  /* Free original */
            
            /* Find new index of correct answer after shuffle */
            s->correct_answer_index = -1;
            if(correct_answer && shuffled_answers) {
                int size = cJSON_GetArraySize(shuffled_answers);
                for(int i = 0; i < size; i++) {
                    cJSON *item = cJSON_GetArrayItem(shuffled_answers, i);
                    if(item && cJSON_IsString(item) && item->valuestring && 
                       strcmp(item->valuestring, correct_answer) == 0) {
                        s->correct_answer_index = i;
                        break;
                    }
                }
                free(correct_answer);
            }
            
            if(shuffled_answers) {
                cJSON_AddItemToObject(json, "answers", shuffled_answers);
            } else {
                cJSON_AddItemToObject(json, "answers", cJSON_CreateArray());
            }
        } else {
            /* Fallback: empty array */
            cJSON_AddItemToObject(json, "answers", cJSON_CreateArray());
            if(answers_json) cJSON_Delete(answers_json);
            s->correct_answer_index = -1;
        }
    } else if(q->type == TRUEFALSE) {
        type_str = "boolean";
        cJSON_AddStringToObject(json, "type", type_str);
    } else if(q->type == FREETEXT) {
        type_str = "text";
        cJSON_AddStringToObject(json, "type", type_str);
    }

    /* Convert to string */
    char *json_string = cJSON_Print(json);
    if(!json_string) {
        cJSON_Delete(json);
        return;
    }

    /* Build response with route */
    char response[2048] = {'\0'};
    int retour_snp = snprintf(response, sizeof(response), "POST question/new\n%s\n\n", json_string);
    
    if(retour_snp < 0 || retour_snp >= (int)sizeof(response)) {
        free(json_string);
        cJSON_Delete(json);
        return;
    }

    /* Send to all active players */
    for(int i = 0; i < clist_size(s->players); i++){
        client *c = (client *)clist_get(s->players, i);
        if(c && (s->type == CLASSIC || c->infos_session.lives >= 0)) {
            send_response(c, response);
        }
    }

    free(json_string);
    cJSON_Delete(json);
}
