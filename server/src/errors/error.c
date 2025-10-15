#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "error.h"

void throw_error(errors error_code, char **optional_string){
    print_error();
    switch (error_code)
    {
    case MEMORY_ALLOCATION:
        printf("memory allocation. ");
        break;
    
    default:
        break;
    }

    print_msg(optional_string);
}

void print_error(){
    printf(WHITE("[")RED("ERROR")WHITE("] "));
}
void print_msg(char **optional_msg){
    if(optional_msg == NULL){
        printf("\n");
        return;
    }
    printf(YELLOW("%s\n"), optional_msg);
}

void debug_log(char *){
    
}