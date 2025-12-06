#pragma once

#include "../../network/client.h"
#include "../../network/network.h"

/**
 * @file login_player.h
 * @brief Player authentication endpoint.
 * 
 * @authors
 * Sebastian Caron
 * Lina Lethoor
 */

/**
 * @brief Handles POST player/login requests.
 * 
 * Authenticates a player by verifying their pseudo and password
 * against the database. Passwords are compared using SHA-256 hashes.
 * 
 * @param s Pointer to the server.
 * @param request JSON request containing "pseudo" and "password".
 * @param cl Pointer to the client.
 * @return 0 on success, 1 on failure.
 */
int post_player_login(server *s, char *request, client *cl);
