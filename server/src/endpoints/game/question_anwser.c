#include <string.h>
#include <stdlib.h>

#include "question_answer.h"
#include "../endpoints.h"
#include "../../json/json.h"
#include "../../json/cJSON.h"

void post_question_answer(session *s, char *request, client *cl) {
    if(!s || !cl) return;
    
    /* Cannot answer twice */
    if(cl->infos_session.has_answered) {
        send_invalid_response(cl);
        return;
    }
    
    /* Parse JSON request */
    cJSON *json = cJSON_Parse(request);
    if(!json) {
        send_invalid_response(cl);
        return;
    }
    
    /* Extract response time if provided */
    float response_time = 0.0;
    cJSON *response_time_item = cJSON_GetObjectItemCaseSensitive(json, "responseTime");
    if(response_time_item && cJSON_IsNumber(response_time_item)) {
        response_time = (float)response_time_item->valuedouble;
    }
    
    /* Get the answer */
    cJSON *answer_item = cJSON_GetObjectItemCaseSensitive(json, "answer");
    if(!answer_item) {
        cJSON_Delete(json);
        send_invalid_response(cl);
        return;
    }
    
    question_type q_type = s->current_question.type;
    
    /* Process answer based on question type */
    if(q_type == QCM) {
        /* QCM: expect integer index */
        if(cJSON_IsNumber(answer_item)) {
            int answer_index = answer_item->valueint;
            cl->infos_session.player_answer.answer_other = (char)answer_index;
            strcpy(cl->infos_session.player_answer.answer_text, "");
        } else {
            cJSON_Delete(json);
            send_invalid_response(cl);
            return;
        }
    } else if(q_type == TRUEFALSE) {
        /* True/False: expect boolean */
        if(cJSON_IsTrue(answer_item)) {
            cl->infos_session.player_answer.answer_other = 1;
            strcpy(cl->infos_session.player_answer.answer_text, "");
        } else if(cJSON_IsFalse(answer_item)) {
            cl->infos_session.player_answer.answer_other = 0;
            strcpy(cl->infos_session.player_answer.answer_text, "");
        } else {
            cJSON_Delete(json);
            send_invalid_response(cl);
            return;
        }
    } else if(q_type == FREETEXT) {
        /* Free text: expect string */
        if(cJSON_IsString(answer_item) && answer_item->valuestring) {
            strncpy(cl->infos_session.player_answer.answer_text, 
                   answer_item->valuestring, 
                   sizeof(cl->infos_session.player_answer.answer_text) - 1);
            cl->infos_session.player_answer.answer_text[sizeof(cl->infos_session.player_answer.answer_text) - 1] = '\0';
            cl->infos_session.player_answer.answer_other = 0;
        } else {
            cJSON_Delete(json);
            send_invalid_response(cl);
            return;
        }
    } else {
        cJSON_Delete(json);
        send_invalid_response(cl);
        return;
    }
    
    /* Record response time and mark as answered */
    cl->infos_session.player_answer.response_time = response_time;
    cl->infos_session.has_answered = 1;
    
    /* Build success response */
    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddStringToObject(response_json, "action", "question/answer");
    cJSON_AddStringToObject(response_json, "statut", "200");
    cJSON_AddStringToObject(response_json, "message", "answer received");
    
    char *json_string = cJSON_Print(response_json);
    if(json_string) {
        char response[1024] = {'\0'};
        snprintf(response, sizeof(response), "%s", json_string);
        send_response(cl, response);
        free(json_string);
    }
    
    cJSON_Delete(response_json);
    cJSON_Delete(json);
}
