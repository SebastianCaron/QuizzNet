#pragma once

#include "session_objects.h"
#include "../network/network.h"

#define SESSION_START_COOLDOWN 3 // in seconds
#define SESSION_QUESTION_COOLDOWN 5 // in seconds

void *handle_session(void *session);

session_type get_session_type(char* mode);

difficulty get_session_difficulty(char* difficulty);

const char* difficulty_to_string(difficulty d);
const char* session_type_to_string(session_type t);
const char* session_status_to_string(session_status s);

void session_destroy(session *);

session* get_session_by_id(chained_list* l_sessions, int id_session);

void session_receive_for_player(session *s, int i);