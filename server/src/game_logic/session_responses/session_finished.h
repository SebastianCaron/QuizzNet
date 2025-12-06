#pragma once

#include "../session_objects.h"
#include "../../network/client.h"

/**
 * @file session_finished.h
 * @brief End-of-game results and ranking.
 * 
 * @authors
 * Sebastian Caron
 * Lina Lethoor
 */

/**
 * @brief Sends final game results to all players.
 * 
 * Calculates final rankings based on game mode:
 * - CLASSIC: Sorted by score, then by correct answers
 * - BATTLE: Sorted by remaining lives, then by elimination order
 * 
 * Broadcasts the final ranking and winner to all players.
 * 
 * @param s Pointer to the session.
 */
void send_session_finished(session *s);
