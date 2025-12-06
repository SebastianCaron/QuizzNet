#pragma once

#include "../../network/client.h"
#include "../../game_logic/session.h"

/**
 * @file list_session.h
 * @brief Session listing endpoint.
 * 
 * @authors
 * Sebastian Caron
 * Lina Lethoor
 */

/**
 * @brief Handles GET session/list requests.
 * 
 * Returns a JSON array of all available game sessions with their
 * details (id, name, themes, difficulty, players, status, etc.).
 * 
 * @param s Pointer to the server.
 * @param request The request string (unused for GET).
 * @param cl Pointer to the client.
 * @return 0 on success, 1 on failure.
 */
int get_session_list(server* s, char* request, client *cl);
