#include <string.h>
#include <stdlib.h>
#include <strings.h>

#include "session_results.h"
#include "../../endpoints/endpoints.h"
#include "../../utils/chained_list.h"
#include "../../json/cJSON.h"
#include "../session.h"

int get_base_points(difficulty diff) {
    switch(diff) {
        case EASY: return 5;
        case MEDIUM: return 10;
        case HARD: return 15;
        default: return 5;
    }
}

int get_bonus_points(difficulty diff){
    switch(diff) {
        case EASY: return 1;
        case MEDIUM: return 3;
        case HARD: return 6;
        default: return 1;
    }
}

int is_answer_correct(session *s, client *cl) {
    question *q = &s->current_question;
    
    if(!cl->infos_session.has_answered) {
        return 0;
    }
    
    if(q->type == QCM) {
        if(s->correct_answer_index < 0) return 0;
        return (cl->infos_session.player_answer.answer_other == s->correct_answer_index) ? 1 : 0;
    } else if(q->type == TRUEFALSE) {
        int correct_bool = 0;
        if(strcmp(q->answer, "true") == 0 || strcmp(q->answer, "1") == 0) {
            correct_bool = 1;
        }
        return (cl->infos_session.player_answer.answer_other == correct_bool) ? 1 : 0;
    } else if(q->type == FREETEXT) {
        if(strlen(cl->infos_session.player_answer.answer_text) == 0) return 0;
        
        cJSON *json = cJSON_Parse(q->answer);
        if(json && cJSON_IsArray(json)) {
            int array_size = cJSON_GetArraySize(json);
            for(int i = 0; i < array_size; i++) {
                cJSON *item = cJSON_GetArrayItem(json, i);
                if(item && cJSON_IsString(item) && item->valuestring) {
                    if(strcasecmp(cl->infos_session.player_answer.answer_text, item->valuestring) == 0) {
                        cJSON_Delete(json);
                        return 1;
                    }
                }
            }
            cJSON_Delete(json);
        } else {
            if(json) cJSON_Delete(json);
            return (strcasecmp(cl->infos_session.player_answer.answer_text, q->answer) == 0) ? 1 : 0;
        }
        return 0;
    }
    
    return 0;
}

int calculate_points(session *s, client *cl, int is_correct) {
    if(!is_correct) return 0;
    
    int base_points = get_base_points(s->current_question.diff);
    
    if(s->type == CLASSIC && cl->infos_session.player_answer.response_time > 0) {
        float half_time = s->time_limit / 2.0f;
        if(cl->infos_session.player_answer.response_time <= half_time) {
            base_points += get_bonus_points(s->current_question.diff);
        }
    }
    
    return base_points;
}

static int get_correct_answer_value(session *s) {
    question *q = &s->current_question;
    
    if(q->type == QCM) {
        return s->correct_answer_index;
    } else if(q->type == TRUEFALSE) {
        return (strcmp(q->answer, "true") == 0 || strcmp(q->answer, "1") == 0) ? 1 : 0;
    }
    
    return -1;
}

char* get_explanation(session *s) {
    return "Réponse correcte";
}

void send_player_eliminated(session *s, client *cl) {
    if(!s || !cl) return;
    
    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "pseudo", cl->pseudo ? cl->pseudo : "");
    
    char *json_string = cJSON_Print(json);
    if(json_string) {
        char response[1024] = {'\0'};
        snprintf(response, sizeof(response), "POST session/player/eliminated\n%s", json_string);
        
        for(int i = 0; i < clist_size(s->players); i++){
            client *c = (client *)clist_get(s->players, i);
            if(c && c->infos_session.lives > 0) {
                send_response(c, response);
            }
        }
        
        free(json_string);
    }
    
    cJSON_Delete(json);
}

void send_session_results(session *s, int question_num) {
    if(!s) return;
    
    question *q = &s->current_question;
    
    client *last_player = NULL;
    float max_time = -1.0f;
    
    if(s->type == BATTLE) {
        for(int i = 0; i < clist_size(s->players); i++){
            client *c = (client *)clist_get(s->players, i);
            if(c && c->infos_session.lives > 0 && c->infos_session.has_answered && 
               c->infos_session.player_answer.response_time > max_time) {
                max_time = c->infos_session.player_answer.response_time;
                last_player = c;
            }
        }
    }
    
    cJSON *results_array = cJSON_CreateArray();
    
    for(int i = 0; i < clist_size(s->players); i++){
        client *c = (client *)clist_get(s->players, i);
        if(!c) continue;

        if(c->infos_session.lives == 0) continue;
        
        int is_correct = is_answer_correct(s, c);
        int points = calculate_points(s, c, is_correct);
        
        if(is_correct) {
            c->infos_session.correctAnswers++;
        }
        
        c->infos_session.score += points;
        
        if(s->type == BATTLE) {
            if(!is_correct && c->infos_session.has_answered) {
                if(c->infos_session.lives > 0) {
                    c->infos_session.lives--;
                }
            }
            if(c == last_player && c->infos_session.has_answered) {
                if(c->infos_session.lives > 0) {
                    c->infos_session.lives--;
                }
            }
        }
        
        cJSON *player_result = cJSON_CreateObject();
        cJSON_AddStringToObject(player_result, "pseudo", c->pseudo ? c->pseudo : "");
        
        if(c->infos_session.has_answered) {
            if(q->type == QCM) {
                cJSON_AddNumberToObject(player_result, "answer", c->infos_session.player_answer.answer_other);
            } else if(q->type == TRUEFALSE) {
                cJSON_AddBoolToObject(player_result, "answer", c->infos_session.player_answer.answer_other == 1);
            } else {
                cJSON_AddStringToObject(player_result, "answer", c->infos_session.player_answer.answer_text);
            }
        } else {
            if(q->type == QCM || q->type == TRUEFALSE) {
                cJSON_AddNumberToObject(player_result, "answer", -1);
            } else {
                cJSON_AddStringToObject(player_result, "answer", "");
            }
        }
        
        cJSON_AddBoolToObject(player_result, "correct", is_correct == 1);
        cJSON_AddNumberToObject(player_result, "points", points);
        cJSON_AddNumberToObject(player_result, "totalScore", c->infos_session.score);
        
        if(s->type == BATTLE) {
            cJSON_AddNumberToObject(player_result, "responseTime", c->infos_session.player_answer.response_time);
            cJSON_AddNumberToObject(player_result, "lives", c->infos_session.lives);
        }
        
        cJSON_AddItemToArray(results_array, player_result);
        
        if(s->type == BATTLE && c->infos_session.lives == 0 && c->infos_session.eliminatedAt == 0) {
            c->infos_session.eliminatedAt = question_num;
            send_player_eliminated(s, c);
        }
    }
    
    cJSON *response_json = cJSON_CreateObject();
    
    if(q->type == QCM || q->type == TRUEFALSE) {
        int correct_answer_value = get_correct_answer_value(s);
        if(correct_answer_value >= 0) {
            cJSON_AddNumberToObject(response_json, "correctAnswer", correct_answer_value);
        }
    }else if(q->type == FREETEXT) {
        cJSON *json = cJSON_Parse(q->answer);
        if(json && cJSON_IsArray(json)) {
            
            cJSON_AddItemToObject(response_json, "correctAnswer", json);
        } else {
            
            cJSON_AddStringToObject(response_json, "correctAnswer", q->answer);
            if(json) cJSON_Delete(json);
        }
    }
    
    char *explanation = get_explanation(s);
    cJSON_AddStringToObject(response_json, "explanation", explanation);
    
    if(s->type == BATTLE && last_player && last_player->pseudo) {
        cJSON_AddStringToObject(response_json, "lastPlayer", last_player->pseudo);
    }
    
    cJSON_AddItemToObject(response_json, "results", results_array);
    
    char *json_string = cJSON_Print(response_json);
    if(json_string) {
        char response[4096] = {'\0'};
        snprintf(response, sizeof(response), "POST question/results\n%s", json_string);
        
        for(int i = 0; i < clist_size(s->players); i++){
            client *c = (client *)clist_get(s->players, i);
            if(c && c->infos_session.lives > 0) {
                send_response(c, response);
            }
        }
        
        free(json_string);
    }
    
    cJSON_Delete(response_json);
}

