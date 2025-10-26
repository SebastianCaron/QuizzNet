#pragma once

#include "session_objects.h"
#include "../network/network.h"

#define SESSION_START_COOLDOWN 3 // in seconds

void *handle_session(void *session);

session_type get_session_type(char* mode);

difficulty get_session_difficulty(char* difficulty);

void session_destroy(session *);