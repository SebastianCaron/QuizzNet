#pragma once

#include "db.h"
#include "../game_logic/session_objects.h"

/**
 * @file requestsdb.h
 * @brief Database queries for question retrieval.
 * 
 * This module provides functions to fetch question data
 * from the database by ID.
 * 
 * @authors
 * Sebastian Caron
 * Lina Lethoor
 */

/**
 * @brief Retrieves the statement text of a question.
 * 
 * @param s Pointer to the server (contains db handle).
 * @param id Question ID to look up.
 * @return Newly allocated string with the statement, or NULL on error.
 * @note The caller must free the returned string.
 */
char *get_statement(server *s, int id);

/**
 * @brief Retrieves the answers of a question.
 * 
 * @param s Pointer to the server (contains db handle).
 * @param id Question ID to look up.
 * @return Newly allocated string with the answers (JSON format), or NULL on error.
 * @note The caller must free the returned string.
 */
char *get_answers(server *s, int id);

/**
 * @brief Retrieves the type of a question.
 * 
 * @param s Pointer to the server (contains db handle).
 * @param id Question ID to look up.
 * @return Question type as integer (0=QCM, 1=TRUEFALSE, 2=FREETEXT), or -1 on error.
 */
int get_type(server *s, int id);

/**
 * @brief Retrieves a complete question by ID.
 * 
 * Populates a question structure with all data from the database.
 * 
 * @param s Pointer to the server (contains db handle).
 * @param id Question ID to look up.
 * @param q Pointer to question structure to populate.
 * @return 0 on success, -1 on error.
 */
int get_question(server *s, int id, question *q);
