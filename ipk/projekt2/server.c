/**
 * IPK proj2 - Server / Client
 * Author: Roman Blanco, xblanc01@stud.fit.vutbr.cz
 * Date: 4.5.2014
 * File: server.c
 */

#include "server.h"

int main(int argc, char *argv[])
{
    int serverPort;
    int serverSpeed; // (kB/s)

    // load arguments -p [PORT] and -d [SPEED]
    if (argc != 5) {
        fprintf(stderr, "Bad params\n");
        return EXIT_FAILURE;
    } else {
        for (int i = 1; i <= 3; i+=2) {
            if (strcmp(argv[i], "-p") == 0) {
                serverPort = atoi(argv[i+1]);
            } else if (strcmp(argv[i], "-d") == 0) {
                serverSpeed = atoi(argv[i+1]);
            } else {
                fprintf(stderr, "Bad params\n");
                return EXIT_FAILURE;
            }
        }
    }
    (void) serverSpeed; // TODO
    connection(serverPort);
    return EXIT_SUCCESS;
}

/**
 * Function: connection
 * --------------------
 * Create socket, bind socket to port and listen for
 * clients (waiting for incomming connection).
 * When client is connected, fork process and start
 * communication between server and client
 *
 * @param port  port on which the server is running
 */

void connection(int port)
{
    int socketDesc;
    int socketAccept;
    socklen_t addrlen;
    struct sockaddr_in server;
    struct sockaddr_in client;
    pid_t clientProcId;

    // create socket
    if ((socketDesc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        fprintf(stderr, "Can't create socket\n");
        exit(-1);
    }
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    // bind socket to port
    if (bind(socketDesc, (struct sockaddr *)&server, sizeof(server)) < 0) {
        close(socketDesc);
        fprintf(stderr, "Error on binding\n");
        exit(-1);
    }
    // listen for clients (wait for incoming connection)
    if (listen(socketDesc, BACKLOG) < 0) {
        close(socketDesc);
        fprintf(stderr, "Error on listening\n");
        exit(-1);
    }
    // WTF man: addrlen is not used, and should also be NULL.
    addrlen = sizeof(client); // addrlen (in accept())
    while (true) {
        if ((socketAccept = accept(socketDesc, (struct sockaddr *)&server,
                        &addrlen)) < 0) {
            fprintf(stderr, "Error on accept\n");
            exit(-1);
        }
        clientProcId = fork();
        if (clientProcId < 0) {
            fprintf(stderr, "Error on fork\n");
            exit(-1);
        } else if (clientProcId == 0) {
            // child process (handles communication)
            close(socketDesc);
            communication(socketAccept);
            exit(0);
        } else {
            // parent process (close current communication socket and 
            // wait for next connection)
            close(socketAccept);
        }
    }
}

/**
 * Function: communication
 * -----------------------
 * Load data from socket sent by client,
 * load, and send requested file
 *
 * @param socket  communication socket
 */ 

void communication(int socket)
{ 
    Tcommunication msg;

    // empty buffer
    memset(msg.incoming, 0, sizeof(msg.incoming));
    memset(msg.outgoing, 0, sizeof(msg.outgoing));
    // read from socket
    // msg.incoming -- filename to upload sent by client
    if (recv(socket, msg.incoming, BUFFSIZE-1, 0) < 0) {
        fprintf(stderr, "Error on reading from socket\n");
        exit(-1);
    }
    // load file data
    //FILE *file = fopen(msg.incoming, "r");
    while (true) {
        int bytesRead = read(0, msg.outgoing, sizeof(msg.outgoing));
        if (bytesRead == 0) {
            // file is loaded
            break;
        } else if (bytesRead < 0) {
            fprintf(stderr, "Error on reading file\n");
            exit(-1);
        }
        // send data over socket
        void *data = msg.outgoing;
        while (bytesRead > 0) {
            int bytesWritten = write(socket, data, bytesRead);
            if (bytesWritten <= 0) {
                fprintf(stderr, "Error on writing data to socket\n");
                exit(-1);
            }
            bytesRead -= bytesWritten;
            //data += bytesWritten;
            // usleep() // speed
        }
    }
}
