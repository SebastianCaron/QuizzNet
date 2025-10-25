#include <stdlib.h>

#include "json.h"
#include "cJSON.h"
#include "../errors/error.h"

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

int* get_from_json_int_array(cJSON *json, char* key){
    cJSON *value = cJSON_GetObjectItemCaseSensitive(json, key);
    int *result;
    int size;

    if(cJSON_IsArray(value) && (size = cJSON_GetArraySize(value)) != 0){
        result = malloc(size*sizeof(int));
        if (!result){
            throw_error(MEMORY_ALLOCATION, "Erreur allocation get_from_json_int_array");
            return NULL;
        }

        for (int i = 0; i < size; i++) {
            cJSON *item = cJSON_GetArrayItem(value, i);
            if (cJSON_IsNumber(item)) {
                result[i] = item->valueint;
            } else {
                result[i] = -1;
            }
        }
        return result;

    }
    return NULL;
}

