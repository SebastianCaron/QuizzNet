#pragma once

#include "../../network/client.h"
#include "../../game_logic/session.h"

/**
 * @file question_answer.h
 * @brief Question answer submission endpoint (in-game).
 * 
 * @authors
 * Sebastian Caron
 * Lina Lethoor
 */

/**
 * @brief Handles POST question/answer requests during a game.
 * 
 * Submits a player's answer to the current question.
 * The answer format depends on the question type:
 * - QCM: Integer index of the selected answer
 * - TRUEFALSE: Boolean value (true/false)
 * - FREETEXT: String answer
 * 
 * Also records the response time for scoring purposes.
 * 
 * @param s Pointer to the current session.
 * @param request JSON containing "answer" and optionally "responseTime".
 * @param cl Pointer to the client.
 */
void post_question_answer(session *s, char *request, client *cl);
