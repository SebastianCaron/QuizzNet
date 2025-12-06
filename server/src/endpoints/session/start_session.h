#pragma once

#include "../../game_logic/session.h"
#include "../endpoints.h"

/**
 * @file start_session.h
 * @brief Session start endpoint.
 * 
 * @authors
 * Sebastian Caron
 * Lina Lethoor
 */

/**
 * @brief Handles POST session/start requests.
 * 
 * Starts a game session. Only the session creator can start the session.
 * Requires at least 2 players to start.
 * Creates a new thread to handle the game loop.
 * 
 * @param s Pointer to the server.
 * @param request The request string.
 * @param cl Pointer to the client (must be session creator).
 * @return 0 on success, 1 on failure.
 */
int post_session_start(server* s, char* request, client *cl);
