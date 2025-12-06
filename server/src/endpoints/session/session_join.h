#pragma once

#include "../../network/client.h"
#include "../../network/network.h"
#include "../../game_logic/session.h"

/**
 * @file session_join.h
 * @brief Session join endpoint.
 * 
 * @authors
 * Sebastian Caron
 * Lina Lethoor
 */

/**
 * @brief Handles POST session/join requests.
 * 
 * Joins a client to an existing game session.
 * Notifies other players in the session about the new player.
 * 
 * Returns appropriate error codes:
 * - 404: Session does not exist
 * - 400: Already in this session
 * - 403: Session is full
 * 
 * @param s Pointer to the server.
 * @param request JSON request containing "sessionId".
 * @param cl Pointer to the client.
 * @return 0 on success, 1 on failure.
 */
int post_session_join(server* s, char* request, client *cl);
