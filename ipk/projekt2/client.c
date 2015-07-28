/**
 * IPK proj2 - Server / Client
 * Author: Roman Blanco, xblanc01@stud.fit.vutbr.cz
 * Date: 4.5.2014
 * File: client.c
 */

#include "client.h"

int main(int argc, char *argv[])
{
    char *hostname;
    char *filename;
    int port;
    int socketDescConnect;

    // load arguments
    if (argc != 2) {
        fprintf(stderr, "Bad params\n");
        return EXIT_FAILURE;
    } else {
        hostname = strtok(argv[1], ":");
        port = atoi(strtok(argv[2], "/"));
        filename = (strtok(argv[2], "/"));
    }
    
    socketDescConnect = connectToServer(hostname, port);
    printf("%s\n", sendMessage(filename, socketDescConnect));

    return EXIT_SUCCESS;
}

/**
 * Function: connectToServer
 * -------------------------
 * Create new socket, convert hostname to IP address
 * and connect to server
 *
 * @param hostname    hostname of server
 * @param port        server port
 * @return socketDesc socket descriptor
 */

int connectToServer(const char *hostname, int port)
{
    int socketDesc;
    struct sockaddr_in server;
    struct hostent *he;

    // create socket
    if ((socketDesc = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "Can't create socket\n");
        return -1;
    }
    // convert hostname to ip
    if ((he = gethostbyname(hostname)) == NULL) {
        fprintf(stderr, "Convert hostname to IP failed\n");
        return -1;
    }
    // connect socket to a server
    memcpy(&server.sin_addr, he->h_addr_list[0], he->h_length);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if (connect(socketDesc, (struct sockaddr *)&server, sizeof(server)) < 0) {
        fprintf(stderr, "Connection error\n");
        return -1;
    }
    return socketDesc;
}

/**
 * Function: sendMessage
 * ---------------------
 * Send message to FTP server and expect answer
 *
 * @param message message that will be sent to FTP server
 * @param socket_desc descriptor of socket that is connected to FTP server
 * @param code code number that is expected in server reply
 * @return string string containing server reply message
 */

const char * sendMessage(const char *message, int socketDesc)
{
    char *serverReply = EMPTY_STRING;
    char tmp[BUFFSIZE] = EMPTY_STRING;

    // send message
    if (send(socketDesc, message, strlen(message), 0) < 0) {
        fprintf(stderr, "Send failed\n");
        return EMPTY_STRING;
    }
    do {
        // empty buffer
        memset(tmp, 0, sizeof(tmp));
        // check if there is data to receive
        if (isResponse(socketDesc) <= 0) {
            fprintf(stderr, "No data in socket to receive\n");
            return EMPTY_STRING;
        }
        // receive reply
        if (recv(socketDesc, tmp, BUFFSIZE - 1, 0) < 0) {
            fprintf(stderr, "Recv failed\n");
            return EMPTY_STRING;
        }
        strcat(serverReply, tmp);
    } while (serverReply != '\0');
    return serverReply;
}

/**
 * Function: isResponse
 * --------------------
 * Check if there are are some data
 * at socket to accept
 *
 * @param socketDesc socket descriptor
 * @return count of socket handles that are ready or
 * 0 if time limit expires
 */

int isResponse(int socketDesc)
{
    fd_set readfds;
    struct timeval time;

    FD_ZERO(&readfds);
    FD_SET(socketDesc, &readfds);
    time.tv_sec = 10;
    time.tv_usec = 0;
    return select((socketDesc + 1), &readfds, NULL, NULL, &time);
}
