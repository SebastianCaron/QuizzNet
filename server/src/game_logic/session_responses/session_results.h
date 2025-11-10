#pragma once

#include "../session_objects.h"
#include "../../network/client.h"

void send_session_results(session *s, int question_num);
void send_player_eliminated(session *s, client *cl);

