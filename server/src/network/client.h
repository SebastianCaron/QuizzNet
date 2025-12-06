#pragma once

#include "../game_logic/session_objects.h"
#include "../utils/buffer_requests.h"

/**
 * @file client.h
 * @brief Client structure definitions and management.
 * 
 * This file contains the data structures representing a connected client,
 * including their connection state, authentication info, and game session data.
 * 
 * @authors
 * Sebastian Caron
 * Lina Lethoor
 */

/**
 * @def NB_LIVES
 * @brief Default number of lives for a player in elimination mode.
 */
#define NB_LIVES 4

/**
 * @struct answer
 * @brief Stores a player's answer to a question.
 * 
 * @param answer_text The text content of the answer (for open questions).
 * @param answer_other Alternative answer indicator.
 * @param response_time Time taken to answer in seconds.
 */
typedef struct {
    char answer_text[512];
    char answer_other;
    float response_time;
} answer;

/**
 * @struct infos_session_client
 * @brief Session-specific information for a client in a game.
 * 
 * @param score Current score of the player.
 * @param joker_pass Whether the pass joker is still available (1) or used (0).
 * @param joker_5050 Whether the 50/50 joker is still available (1) or used (0).
 * @param is_creator Whether this client created the session (1) or joined (0).
 * @param session Pointer to the session the client is participating in.
 * @param has_answered Whether the client has answered the current question.
 * @param skip Whether the client skipped the current question.
 * @param lives Remaining lives in elimination mode.
 * @param correctAnswers Total number of correct answers.
 * @param eliminatedAt Question number at which the player was eliminated (-1 if still playing).
 * @param player_answer The player's current answer data.
 */
typedef struct {
    int score;
    char joker_pass;
    char joker_5050;
    char is_creator;
    session *session;
    char has_answered;
    char skip;
    char lives;
    int correctAnswers;
    int eliminatedAt;

    answer player_answer;
} infos_session_client;

/**
 * @struct client
 * @brief Represents a connected client.
 * 
 * @param fd Socket file descriptor for communication.
 * @param connected Authentication status (1 = logged in, 0 = anonymous).
 * @param pseudo Player's username (NULL if not logged in).
 * @param infos_session Session-related data for the current game.
 * @param buffer_cl Buffer for accumulating incoming data from this client.
 */
typedef struct {
    int fd;
    char connected;
    char* pseudo;
    infos_session_client infos_session;
    buffer buffer_cl;
} client;

/**
 * @brief Creates and initializes a new client structure.
 * 
 * Allocates memory for a client and sets default values.
 * The client starts as not connected (anonymous) with full lives.
 * 
 * @param fd Socket file descriptor for the client connection.
 * @return Pointer to the new client, or NULL on allocation failure.
 */
client *client_init(int fd);

/**
 * @brief Frees all resources associated with a client.
 * 
 * Closes the socket connection and frees allocated memory.
 * 
 * @param c Pointer to the client to destroy.
 */
void client_destroy(client *c);
