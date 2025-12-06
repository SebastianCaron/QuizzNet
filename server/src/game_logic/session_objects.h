#pragma once

#include "../network/server.h"
#include "../utils/chained_list.h"

/**
 * @file session_objects.h
 * @brief Game session data structures and enumerations.
 * 
 * This file contains all the core data structures used to represent
 * game sessions, questions, and their related enumerations.
 * 
 * @authors
 * Sebastian Caron
 * Lina Lethoor
 */

/**
 * @enum difficulty
 * @brief Question difficulty levels.
 */
typedef enum{
    EASY,       /**< Easy difficulty (5 base points) */
    MEDIUM,     /**< Medium difficulty (10 base points) */
    HARD        /**< Hard difficulty (15 base points) */
} difficulty;

/**
 * @enum session_type
 * @brief Game mode types.
 */
typedef enum{
    CLASSIC,    /**< Classic mode: score-based competition */
    BATTLE      /**< Battle mode: elimination with lives */
} session_type;

/**
 * @enum session_status
 * @brief Current state of a game session.
 */
typedef enum{
    WAITING,    /**< Waiting for players to join */
    PLAYING,    /**< Game in progress */
    FINISHED    /**< Game completed */
} session_status;

/**
 * @enum question_type
 * @brief Types of questions supported.
 */
typedef enum{
    QCM,        /**< Multiple choice question */
    TRUEFALSE,  /**< True/False question */
    FREETEXT    /**< Free text answer question */
} question_type;

/**
 * @struct question
 * @brief Represents a quiz question.
 * 
 * @param id Unique question identifier.
 * @param type Type of question (QCM, TRUEFALSE, FREETEXT).
 * @param diff Difficulty level.
 * @param statement The question text.
 * @param answer The correct answer(s) (JSON array for QCM/FREETEXT, string for TRUEFALSE).
 */
typedef struct {
    int id;
    question_type type;
    difficulty diff;
    char statement[500];
    char answer[500];
} question;

/**
 * @struct session
 * @brief Represents a game session.
 * 
 * @param id Unique session identifier.
 * @param name Session name displayed to players.
 * @param themes_ids Array of theme IDs for question selection.
 * @param nb_themes Number of themes selected.
 * @param difficulty Session difficulty level.
 * @param nb_questions Total number of questions in the session.
 * @param time_limit Time limit per question in seconds.
 * @param type Game mode (CLASSIC or BATTLE).
 * @param nb_lives Initial lives for BATTLE mode.
 * @param nb_players Current number of players.
 * @param max_nb_players Maximum allowed players.
 * @param status Current session status.
 * @param buffer Shared buffer for receiving player data.
 * @param buffer_capacity Buffer allocated capacity.
 * @param buffer_size Current bytes in buffer.
 * @param players Linked list of client pointers.
 * @param server Pointer to the main server.
 * @param current_question The question currently being played.
 * @param correct_answer_index Index of correct answer for QCM (after shuffle).
 */
typedef struct {

    int id;
    char *name;
    int *themes_ids;
    int nb_themes;
    difficulty difficulty;
    int nb_questions;
    int time_limit;
    session_type type;
    int nb_lives;
    int nb_players;
    int max_nb_players;
    session_status status;

    char *buffer;
    int buffer_capacity;
    ssize_t buffer_size;

    chained_list *players;
    server *server;
    
    question current_question;
    int correct_answer_index;
} session;
