#pragma once

#include "../session_objects.h"
#include "../../json/cJSON.h"

void send_session_question(session *s, question *q, int question_num);
void shuffle_cjson_array(cJSON *array);

