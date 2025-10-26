#pragma once

#include "../../network/client.h"
#include "../../game_logic/session.h"

int post_session_create(server* s, char* request, client *cl);