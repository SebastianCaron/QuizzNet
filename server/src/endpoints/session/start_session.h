#pragma once

#include "../../game_logic/session.h"
#include "../endpoints.h"

int post_session_start(server* s, char* request, client *cl);