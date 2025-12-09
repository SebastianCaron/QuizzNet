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
 * @brief Creates a shuffled copy of a cJSON string array.
 * 
 * Used to randomize answer order for QCM questions.
 * The original array is not modified.
 * 
 * @param array cJSON array to shuffle.
 * @return New shuffled cJSON array, or NULL on error. Caller must free with cJSON_Delete.
 */
cJSON *create_shuffled_array(cJSON *array);
