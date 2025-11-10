#pragma once

#include "../../network/client.h"
#include "../../game_logic/session.h"

void post_question_answer(session *s, char *request, client *cl);