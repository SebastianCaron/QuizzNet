#pragma once

#include "../../network/client.h"
#include "../../network/network.h"
#include "../../game_logic/session.h"

int post_session_join(server* s, char* request, client *cl);