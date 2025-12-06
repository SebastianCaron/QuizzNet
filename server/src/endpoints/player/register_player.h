#pragma once

#include "../../network/client.h"
#include "../../network/network.h"

/**
 * @file register_player.h
 * @brief Player registration endpoint.
 * 
 * @authors
 * Sebastian Caron
 * Lina Lethoor
 */

/**
 * @brief Handles POST player/register requests.
 * 
 * Creates a new player account with the given pseudo and password.
 * Password is hashed with SHA-256 before storage.
 * Returns 409 if pseudo already exists.
 * 
 * @param s Pointer to the server.
 * @param request JSON request containing "pseudo" and "password".
 * @param cl Pointer to the client.
 * @return 0 on success, 1 on failure.
 */
int post_player_register(server *s, char *request, client *cl);
