#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>

#define UDP_PORT 5555
#define BUFFER_SIZE 512

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <SERVER_NAME> <PORT_TCP>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *SERVER_NAME = argv[1];
    const char *PORT_TCP = argv[2];

    
}