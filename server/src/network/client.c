#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "client.h"
#include "../errors/error.h"
#include "../utils/buffer_requests.h"

client *client_init(int fd){
    client *res = calloc(1, sizeof(client));

    if(!res){
        throw_error(MEMORY_ALLOCATION, "Erreur allocation pour structure client");
        return NULL;
    }

    /* Initialize client with default values */
    res->fd = fd;
    res->connected = 0;                    /* Not authenticated yet */
    res->infos_session.lives = NB_LIVES;   /* Full lives for elimination mode */
    
    /* Initialize the receive buffer */
    res->buffer_cl.capacity = MAX_BUFFER_SIZE;
    res->buffer_cl.size = 0;
    memset(res->buffer_cl.buffer, 0, MAX_BUFFER_SIZE + 1);
    
    return res;
}

void client_destroy(client *c){
    if(!c) return;

    /* Close socket and free memory */
    close(c->fd);
    free(c);
}
