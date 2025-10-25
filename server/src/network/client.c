#include <stdlib.h>
#include <unistd.h>


#include "client.h"
#include "../errors/error.h"

client *client_init(int fd){
    client *res = calloc(1, sizeof(client));

    if(!res){
        throw_error(MEMORY_ALLOCATION, "Erreur allocation pour structure client");
        return NULL;
    }

    res->fd = fd;
    res-> connected = false;
    return res;
}

void client_destroy(client *c){
    if(!c) return;

    close(c->fd);
    free(c);
}