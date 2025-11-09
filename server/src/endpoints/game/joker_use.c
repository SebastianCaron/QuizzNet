#include <string.h>
#include <stdlib.h>

#include "joker_use.h"
#include "../endpoints.h"
#include "../../json/json.h"
#include "../../json/cJSON.h"
#include "../../game_logic/session_responses/session_question.h"

void post_joker_use(session *s, char *request, client *cl) {
    if(!s || !cl) return;
    
    if(cl->infos_session.has_answered) {
        send_invalid_response(cl);
        return;
    }
    
    cJSON *json = cJSON_Parse(request);
    if(!json) {
        send_invalid_response(cl);
        return;
    }
    
    char *joker_type = get_from_json_string(json, "type");
    if(!joker_type) {
        cJSON_Delete(json);
        send_invalid_response(cl);
        return;
    }
    
    if(strcmp(joker_type, "fifty") == 0) {
        if(cl->infos_session.joker_5050 == 0) {
            cJSON_Delete(json);
            send_invalid_response(cl);
            return;
        }
        if(s->current_question.type != QCM) {
            cJSON_Delete(json);
            send_invalid_response(cl);
            return;
        }
        
        cl->infos_session.joker_5050 = 0;
        
        cJSON *original_answers = cJSON_Parse(s->current_question.answer);
        if(!original_answers || !cJSON_IsArray(original_answers)) {
            cJSON_Delete(json);
            if(original_answers) cJSON_Delete(original_answers);
            send_error_response(cl);
            return;
        }
        
        cJSON *correct_answer_item = cJSON_GetArrayItem(original_answers, 0);
        if(!correct_answer_item || !cJSON_IsString(correct_answer_item)) {
            cJSON_Delete(json);
            cJSON_Delete(original_answers);
            send_error_response(cl);
            return;
        }
        char *correct_answer = correct_answer_item->valuestring;
        
        int size = cJSON_GetArraySize(original_answers);
        if(size < 2) {
            cJSON_Delete(json);
            cJSON_Delete(original_answers);
            send_error_response(cl);
            return;
        }
        
        int other_index = 1 + (rand() % (size - 1));
        cJSON *other_item = cJSON_GetArrayItem(original_answers, other_index);
        if(!other_item || !cJSON_IsString(other_item)) {
            cJSON_Delete(json);
            cJSON_Delete(original_answers);
            send_error_response(cl);
            return;
        }
        
        cJSON *remaining_answers = cJSON_CreateArray();
        if(remaining_answers) {
            cJSON_AddItemToArray(remaining_answers, 
                cJSON_CreateString(correct_answer));
            
            cJSON_AddItemToArray(remaining_answers, 
                cJSON_CreateString(other_item->valuestring));
            
            shuffle_cjson_array(remaining_answers);
            
            cJSON *response_json = cJSON_CreateObject();
            cJSON_AddStringToObject(response_json, "action", "joker/use");
            cJSON_AddStringToObject(response_json, "statut", "200");
            cJSON_AddStringToObject(response_json, "message", "joker activated");
            cJSON_AddItemToObject(response_json, "remainingAnswers", remaining_answers);
            
            cJSON *jokers_obj = cJSON_CreateObject();
            cJSON_AddNumberToObject(jokers_obj, "fifty", cl->infos_session.joker_5050);
            cJSON_AddNumberToObject(jokers_obj, "skip", cl->infos_session.joker_pass);
            cJSON_AddItemToObject(response_json, "jokers", jokers_obj);
            
            char *json_string = cJSON_Print(response_json);
            if(json_string) {
                char response[2048] = {'\0'};
                snprintf(response, sizeof(response), "%s", json_string);
                send_response(cl, response);
                free(json_string);
            }
            
            cJSON_Delete(response_json);
        } else {
            cJSON_Delete(json);
            cJSON_Delete(original_answers);
            send_error_response(cl);
            return;
        }
        
        cJSON_Delete(original_answers);
        
    } else if(strcmp(joker_type, "skip") == 0) {
        if(cl->infos_session.joker_pass == 0) {
            cJSON_Delete(json);
            send_invalid_response(cl);
            return;
        }
        
        cl->infos_session.joker_pass = 0;

        cl->infos_session.has_answered = 1;
        cl->infos_session.skip = 1;

        cJSON *response_json = cJSON_CreateObject();
        cJSON_AddStringToObject(response_json, "action", "joker/use");
        cJSON_AddStringToObject(response_json, "statut", "200");
        cJSON_AddStringToObject(response_json, "message", "question skipped");
        
        cJSON *jokers_obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(jokers_obj, "fifty", cl->infos_session.joker_5050);
        cJSON_AddNumberToObject(jokers_obj, "skip", cl->infos_session.joker_pass);
        cJSON_AddItemToObject(response_json, "jokers", jokers_obj);
        
        char *json_string = cJSON_Print(response_json);
        if(json_string) {
            char response[2048] = {'\0'};
            snprintf(response, sizeof(response), "%s", json_string);
            send_response(cl, response);
            free(json_string);
        }
        
        cJSON_Delete(response_json);
    } else {
        cJSON_Delete(json);
        send_invalid_response(cl);
        return;
    }
    
    cJSON_Delete(json);
}
