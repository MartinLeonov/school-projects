/**
 * IPK proj2 - Server / Client
 * Author: Roman Blanco, xblanc01@stud.fit.vutbr.cz
 * Date: 4.5.2014
 * File: client.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/types.h>

#define BUFFSIZE 1024
#define EMPTY_STRING ""

int connectToServer(const char *hostname, int port);
const char * sendMessage(const char *message, int socketDesc);
int isResponse(int socketDesc);
