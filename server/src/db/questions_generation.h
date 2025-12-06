#pragma once

#include "db.h"

/**
 * @file questions_generation.h
 * @brief Random question selection for game sessions.
 * 
 * This module provides utilities for selecting random questions
 * from the database based on themes and difficulty levels.
 * 
 * @authors
 * Sebastian Caron
 * Lina Lethoor
 */

/**
 * @brief Selects random question IDs matching given criteria.
 * 
 * Queries the database for random questions that match the specified
 * themes and difficulty levels.
 * 
 * @param s Pointer to the server (contains db handle).
 * @param size Number of questions to select.
 * @param tab Output array to store the selected question IDs.
 * @param nb_themes Number of theme IDs in the filter.
 * @param ids_themes Array of theme IDs to filter by.
 * @param nb_difficulties Number of difficulty levels in the filter.
 * @param difficulties Array of difficulty levels to filter by.
 */
void get_random_question_ids(server *s, int size, int *tab, int nb_themes, int *ids_themes, int nb_difficulties, int *difficulties);
