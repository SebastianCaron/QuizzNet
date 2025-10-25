#include <string.h>
#include "session.h"

session_type get_session_type(char* mode){
    return strcmp("solo", mode) ? BATTLE : CLASSIC;
}

difficulty get_session_difficulty(char* difficulty_str){
    if (!(strcmp("moyen", difficulty_str))) return MEDIUM;
    if (!(strcmp("difficile", difficulty_str))) return HARD;
    return EASY;
}

