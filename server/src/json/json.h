#pragma once

#include "cJSON.h"

/**
 * @file json.h
 * @brief JSON utility functions for data extraction.
 * 
 * This module provides helper functions to extract typed values
 * from cJSON objects, simplifying JSON parsing throughout the application.
 * 
 * @authors
 * Sebastian Caron
 * Lina Lethoor
 */

/**
 * @brief Extracts a string value from a JSON object.
 * 
 * @param json Pointer to the cJSON object.
 * @param key Key name to look up.
 * @return The string value if found and valid, NULL otherwise.
 * @note The returned string is owned by the cJSON object and should not be freed.
 */
char *get_from_json_string(cJSON *json, char *key);

/**
 * @brief Extracts an integer value from a JSON object.
 * 
 * @param json Pointer to the cJSON object.
 * @param key Key name to look up.
 * @return The integer value if found, 0 otherwise.
 */
int get_from_json_int(cJSON *json, char *key);

/**
 * @brief Extracts an array of integers from a JSON object.
 * 
 * Allocates and returns a new array containing all integer values.
 * Non-numeric elements in the array are set to -1.
 * 
 * @param json Pointer to the cJSON object.
 * @param key Key name to look up.
 * @return Newly allocated int array if found, NULL otherwise.
 * @note The caller is responsible for freeing the returned array.
 * @warning The size of the array is not returned; caller must track it separately.
 */
int *get_from_json_int_array(cJSON *json, char *key);
