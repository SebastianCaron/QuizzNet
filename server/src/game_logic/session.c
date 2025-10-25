#include <string.h>
#include "session.h"

session_type get_session_type(char* mode){
    return strcmp("solo", mode) ? session_type.BATTLE : session_type.CLASSIC;
}

difficulty get_session_difficulty(char* difficutly){
    if (!(strcmp("moyen", difficulty))) return difficulty.MEDIUM;
    if (!(strcmp("difficile", difficulty))) return difficulty.HARD;
    return difficulty.EASY;
}

