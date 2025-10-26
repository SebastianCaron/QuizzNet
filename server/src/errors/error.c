#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "error.h"

void print_msg(const char *optional_msg){
    if(optional_msg == NULL){
        printf("\n");
        return;
    }
    printf(YELLOW("%s\n"), optional_msg);
}

void print_error(){
    printf(WHITE("[")RED("ERROR")WHITE("] "));
}

void debug_log(char *){
    
}

void throw_error(errors error_code, const char *optional_string){
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

