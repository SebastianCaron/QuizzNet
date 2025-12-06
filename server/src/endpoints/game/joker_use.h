#pragma once

#include "../../network/client.h"
#include "../../game_logic/session.h"

/**
 * @file joker_use.h
 * @brief Joker usage endpoint (in-game).
 * 
 * @authors
 * Sebastian Caron
 * Lina Lethoor
 */

/**
 * @brief Handles POST joker/use requests during a game.
 * 
 * Allows a player to use one of their available jokers:
 * - "fifty": Removes 2 wrong answers (QCM only), returns remaining answers
 * - "skip": Skips the current question without penalty
 * 
 * Each joker can only be used once per game.
 * Cannot be used after the player has already answered.
 * 
 * @param s Pointer to the current session.
 * @param request JSON request containing "type" ("fifty" or "skip").
 * @param cl Pointer to the client.
 */
void post_joker_use(session *s, char *request, client *cl);
