#include "cJSON.h"

char *get_from_json_string(cJSON *json, char *key){
    cJSON *value = cJSON_GetObjectItemCaseSensitive(json, key);
    if (cJSON_IsString(value) && (value->valuestring != NULL)) {
        return value->valuestring;
    }
    return NULL;
}

int get_from_json_int(cJSON *json, char *key){
    cJSON *value = cJSON_GetObjectItemCaseSensitive(json, key);
    if (cJSON_IsNumber(value)) {
        return value->valueint;
    }
    return 0;
}