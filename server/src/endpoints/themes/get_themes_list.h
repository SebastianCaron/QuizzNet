#pragma once

#include "../../network/client.h"
#include "../../network/network.h"

/**
 * @file get_themes_list.h
 * @brief Theme listing endpoint.
 * 
 * @authors
 * Sebastian Caron
 * Lina Lethoor
 */

/**
 * @brief Handles GET themes/list requests.
 * 
 * Returns a JSON array of all available quiz themes
 * with their IDs and names.
 * 
 * @param s Pointer to the server.
 * @param request The request string (unused for GET).
 * @param cl Pointer to the client.
 * @return 0 on success, 1 on failure.
 */
int get_themes_list(server *s, char *request, client *cl);
