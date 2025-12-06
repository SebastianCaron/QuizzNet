#pragma once

#include "../../network/client.h"
#include "../../game_logic/session.h"

/**
 * @file create_session.h
 * @brief Session creation endpoint.
 * 
 * @authors
 * Sebastian Caron
 * Lina Lethoor
 */

/**
 * @brief Handles POST session/create requests.
 * 
 * Creates a new game session with the specified parameters.
 * The client becomes the session creator and is automatically joined.
 * 
 * Expected JSON fields:
 * - name: Session name
 * - themesIds: Array of theme IDs
 * - difficulty: "easy", "medium", or "hard"
 * - nbQuestions: Number of questions
 * - timeLimit: Time limit per question in seconds
 * - mode: "classic" or "battle"
 * - maxPlayers: Maximum number of players
 * 
 * @param s Pointer to the server.
 * @param request JSON request with session parameters.
 * @param cl Pointer to the client (becomes session creator).
 * @return 0 on success, 1 on failure.
 */
int post_session_create(server* s, char* request, client *cl);
