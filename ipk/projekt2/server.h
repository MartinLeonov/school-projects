/**
 * IPK proj2 - Server / Client
 * Author: Roman Blanco, xblanc01@stud.fit.vutbr.cz
 * Date: 4.5.2014
 * File: server.h
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BACKLOG 5 // max connections in queue during listening
#define BUFFSIZE 1024

typedef struct Tcommunication {
    char incoming[BUFFSIZE];
    char outgoing[BUFFSIZE];
} Tcommunication;

void connection(int port);
void communication(int socket);
