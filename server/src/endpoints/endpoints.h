#pragma once

#include "../network/client.h"

/**
 * @file endpoints.h
 * @brief HTTP-like request routing and response handling.
 * 
 * This module provides the main request router that dispatches
 * incoming client requests to the appropriate endpoint handlers.
 * 
 * @authors
 * Sebastian Caron
 * Lina Lethoor
 */

/**
 * @def MAX_ENDPOINT_NAME
 * @brief Maximum length of an endpoint route string.
 */
#define MAX_ENDPOINT_NAME 256

/**
 * @enum endpoints
 * @brief Enumeration of all available API endpoints.
 */
typedef enum {
    INVALID_ENDPOINT,       /**< Unknown or malformed endpoint */
    POST_PLAYER_REGISTER,   /**< POST player/register - Create new account */
    POST_PLAYER_LOGIN,      /**< POST player/login - Authenticate user */
    POST_SESSION_CREATE,    /**< POST session/create - Create game session */
    POST_SESSION_JOIN,      /**< POST session/join - Join game session */
    POST_SESSION_START,     /**< POST session/start - Start a game session */
    GET_SESSION_LIST,       /**< GET session/list - List available sessions */
    GET_THEMES_LIST,        /**< GET themes/list - List quiz themes */
    POST_JOKER_USE,         /**< POST joker/use - Use a joker in game */
    POST_QUESTION_ANSWER    /**< POST question/answer - Submit answer */
} endpoints;

/**
 * @brief Parses a request string and returns the matching endpoint.
 * 
 * @param request The raw request string (e.g., "POST player/login").
 * @return The matching endpoints enum value, or INVALID_ENDPOINT if not found.
 */
endpoints get_endpoint(char *request);

/**
 * @brief Main request dispatcher that routes to appropriate handlers.
 * 
 * Parses the request, identifies the endpoint, and calls the
 * corresponding handler function.
 * 
 * @param s Pointer to the server.
 * @param request The raw request string.
 * @param cl Pointer to the client making the request.
 */
void handle_request(server *s, char *request, client *cl);

/**
 * @brief Sends a 400 Bad Request response to the client.
 * 
 * @param cl Pointer to the client.
 */
void send_invalid_response(client *cl);

/**
 * @brief Sends a 520 Unknown Error response to the client.
 * 
 * @param cl Pointer to the client.
 */
void send_error_response(client *cl);

/**
 * @brief Sends a raw response string to the client.
 * 
 * @param cl Pointer to the client.
 * @param response The response string to send.
 */
void send_response(client *cl, char *response);
