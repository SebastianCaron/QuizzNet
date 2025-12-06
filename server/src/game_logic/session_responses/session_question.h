#pragma once

#include "../session_objects.h"
#include "../../json/cJSON.h"

/**
 * @file session_question.h
 * @brief Question broadcasting to session players.
 * 
 * @authors
 * Sebastian Caron
 * Lina Lethoor
 */

/**
 * @brief Sends a question to all active players in the session.
 * 
 * Builds and broadcasts a JSON message containing the question,
 * its type, difficulty, and answers (shuffled for QCM).
 * Only sends to players with lives > 0.
 * 
 * @param s Pointer to the session.
 * @param q Pointer to the question to send.
 * @param question_num Current question number (1-based).
 */
void send_session_question(session *s, question *q, int question_num);

/**
 * @brief Shuffles a cJSON array in place
 * 
 * Used to randomize answer order for QCM questions.
 * 
 * @param array cJSON array to shuffle.
 */
void shuffle_cjson_array(cJSON *array);
