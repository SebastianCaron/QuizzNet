#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "network/udp.h"
#include "network/network.h"
#include "db/db.h"
#include "db/initdb.h"

#define UDP_PORT 5555

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <SERVER_NAME> <PORT_TCP>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *SERVER_NAME = argv[1];
    const char *PORT_TCP = argv[2];

    start_udp(SERVER_NAME, PORT_TCP);

    server *s = start_server(atoi(argv[2]));
    add_sqlite_to_server(s);
    init_db(s);
    while(1){
        server_client_procedure(s);
    }
}