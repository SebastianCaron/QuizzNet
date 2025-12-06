#pragma once

#include "session_objects.h"
#include "../network/network.h"

/**
 * @file session.h
 * @brief Game session management and game loop.
 * 
 * This module handles the game session lifecycle including
 * starting games, processing questions, and managing players.
 * 
 * @authors
 * Sebastian Caron
 * Lina Lethoor
 */

/**
 * @def SESSION_START_COOLDOWN
 * @brief Delay in seconds before the game starts after session/start.
 */
#define SESSION_START_COOLDOWN 3

/**
 * @def SESSION_QUESTION_COOLDOWN
 * @brief Delay in seconds between questions (after results are shown).
 */
#define SESSION_QUESTION_COOLDOWN 5

/**
 * @brief Main game loop function, runs in its own thread.
 * 
 * Handles the entire game flow: sending questions, receiving answers,
 * calculating results, and determining the winner.
 * 
 * @param session Pointer to the session structure.
 * @return NULL when the game ends.
 */
void *handle_session(void *session);

/**
 * @brief Converts a mode string to session_type enum.
 * 
 * @param mode Mode string ("solo" or "battle").
 * @return CLASSIC for "solo", BATTLE otherwise.
 */
session_type get_session_type(char* mode);

/**
 * @brief Converts a difficulty string to difficulty enum.
 * 
 * @param difficulty Difficulty string ("facile", "moyen", "difficile").
 * @return Corresponding difficulty enum value.
 */
difficulty get_session_difficulty(char* difficulty);

/**
 * @brief Converts difficulty enum to display string.
 * 
 * @param d Difficulty enum value.
 * @return French string representation ("facile", "moyen", "difficile").
 */
const char* difficulty_to_string(difficulty d);

/**
 * @brief Converts session_type enum to display string.
 * 
 * @param t Session type enum value.
 * @return String representation ("solo", "battle").
 */
const char* session_type_to_string(session_type t);

/**
 * @brief Converts session_status enum to display string.
 * 
 * @param s Session status enum value.
 * @return String representation ("waiting", "playing", "finished").
 */
const char* session_status_to_string(session_status s);

/**
 * @brief Frees all resources associated with a session.
 * 
 * Re-attaches players to the main server loop before cleanup.
 * 
 * @param s Pointer to the session to destroy.
 */
void session_destroy(session *s);

/**
 * @brief Finds a session by its ID.
 * 
 * @param l_sessions Linked list of sessions to search.
 * @param id_session Session ID to find.
 * @return Pointer to the session, or NULL if not found.
 */
session* get_session_by_id(chained_list* l_sessions, int id_session);

/**
 * @brief Receives and processes data from a player in the session.
 * 
 * Handles incoming requests (answers, joker usage) during the game.
 * 
 * @param s Pointer to the session.
 * @param i Index of the player in the players list.
 */
void session_receive_for_player(session *s, int i);

/**
 * @brief Removes a client from a session.
 * 
 * Handles creator transfer if the creator leaves.
 * Destroys the session if it becomes empty and not playing.
 * 
 * @param s Pointer to the session.
 * @param cl Pointer to the client to remove.
 */
void session_remove_client(session *s, client *cl);
