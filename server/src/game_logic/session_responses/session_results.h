#pragma once

#include "../session_objects.h"
#include "../../network/client.h"

/**
 * @file session_results.h
 * @brief Question results calculation and broadcasting.
 * 
 * @authors
 * Sebastian Caron
 * Lina Lethoor
 */

/**
 * @brief Calculates and sends results for the current question.
 * 
 * Evaluates each player's answer, calculates points, updates scores,
 * and handles BATTLE mode eliminations (wrong answer or slowest player
 * loses a life).
 * 
 * @param s Pointer to the session.
 * @param question_num Current question number for elimination tracking.
 */
void send_session_results(session *s, int question_num);

/**
 * @brief Notifies all players that a player has been eliminated.
 * 
 * Broadcasts an elimination message to all remaining players.
 * 
 * @param s Pointer to the session.
 * @param cl Pointer to the eliminated client.
 */
void send_player_eliminated(session *s, client *cl);
