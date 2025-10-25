#pragma once

#include "cJSON.h"
char *get_from_json_string(cJSON *json, char *key);
int get_from_json_int(cJSON *json, char *key);