#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "session_question.h"
#include "../../endpoints/endpoints.h"
#include "../../utils/chained_list.h"
#include "../../json/cJSON.h"
#include "../session.h"

void shuffle_cjson_array(cJSON *array) {
    if(!array || !cJSON_IsArray(array)) return;
    
    int size = cJSON_GetArraySize(array);
    if(size <= 1) return;
    
    for(int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        if(i != j) {
            cJSON *item_i = cJSON_GetArrayItem(array, i);
            cJSON *item_j = cJSON_GetArrayItem(array, j);
            
            if(item_i && item_j && cJSON_IsString(item_i) && cJSON_IsString(item_j)) {
                char *temp = item_i->valuestring;
                item_i->valuestring = item_j->valuestring;
                item_j->valuestring = temp;
            }
        }
    }
}

void send_session_question(session *s, question *q, int question_num){
    if(!s || !q) return;

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
        
        cJSON *answers_json = cJSON_Parse(q->answer);
        if(answers_json && cJSON_IsArray(answers_json)) {
            cJSON *correct_answer_item = cJSON_GetArrayItem(answers_json, 0);
            char *correct_answer = NULL;
            if(correct_answer_item && cJSON_IsString(correct_answer_item)) {
                correct_answer = correct_answer_item->valuestring;
            }
            
            shuffle_cjson_array(answers_json);
            
            s->correct_answer_index = -1;
            if(correct_answer) {
                int size = cJSON_GetArraySize(answers_json);
                for(int i = 0; i < size; i++) {
                    cJSON *item = cJSON_GetArrayItem(answers_json, i);
                    if(item && cJSON_IsString(item) && item->valuestring && 
                       strcmp(item->valuestring, correct_answer) == 0) {
                        s->correct_answer_index = i;
                        break;
                    }
                }
            }
            
            cJSON_AddItemToObject(json, "answers", answers_json);
        } else {
            cJSON *empty_array = cJSON_CreateArray();
            cJSON_AddItemToObject(json, "answers", empty_array);
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

    char *json_string = cJSON_Print(json);
    if(!json_string) {
        cJSON_Delete(json);
        return;
    }

    char response[2048] = {'\0'};
    int retour_snp = snprintf(response, sizeof(response), "POST question/new\n%s", json_string);
    
    if(retour_snp < 0 || retour_snp >= (int)sizeof(response)) {
        free(json_string);
        cJSON_Delete(json);
        return;
    }

    for(int i = 0; i < clist_size(s->players); i++){
        client *c = (client *)clist_get(s->players, i);
        if(c && c->infos_session.lives > 0) {
            send_response(c, response);
        }
    }

    free(json_string);
    cJSON_Delete(json);
}

