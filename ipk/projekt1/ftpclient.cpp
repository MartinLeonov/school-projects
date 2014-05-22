#include <iostream>
#include <string>
#include <regex>

#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define EMPTY_STRING ""
#define BUFSIZE 1024
#define FTP_PORT 21
#define USER 1
#define PASS 2
#define HOST 3
#define PORT 4
#define PATH 5
#define CHECK_REPLY \
    if (reply.empty()) { \
        close(socketDescConnect); \
        return 1; \
    } 

using namespace std;

smatch match(string pattern, string source);
string sendMessage(string message, int socket_desc, string code);
int isResponse(int socketDesc);
int connectToFtp(const char *argv, int port);
int stringToInt(string number);

int main(int argc, char *argv[])
{
    int socketDescConnect;
    int socketDescTransfer;
    smatch addressParams;
    string address[6];
    string message;
    string reply;

    // check if is set parameter with ftp address
    if (argc != 2) {
        cerr << "Bad params" << endl;
        return 1;
    }

    // regex to split address from argument
    string regex = "(?:ftp:\\/\\/)?(?:ftp:\\/\\/(\\w+)(?:\\:)(\\w+)"
                   "(?:@)?)?((?:\\.|\\w+)+)"
                   "(?:(?:\\:)?(\\d+)?)?((?:\\/\\S+)+\\/?$)?";  

    // matching address
    // TODO check if parameter is correct 
    addressParams = match(regex, argv[1]);
    if (addressParams.empty()) {
        cerr << "wrong format of FTP address entered" << endl;
        return 1;
    }

    // load address parameters to array
    // TODO edit
    for (unsigned int i = 0; i < addressParams.size(); ++i) {
        address[i] = addressParams[i].str();
    }

    // connect to server 
    if (address[PORT] == EMPTY_STRING) {
        socketDescConnect = connectToFtp(address[HOST].c_str(), FTP_PORT);
    } else {
        socketDescConnect = connectToFtp(address[HOST].c_str(), 
            stringToInt(address[PORT]));
    }
    if (socketDescConnect == -1) {
        return 1;
    }

    // send empty string to check if server is FTP
    reply = sendMessage("", socketDescConnect, "220");
    CHECK_REPLY;

    // send username
    (address[USER] == EMPTY_STRING) ?
        message = "USER anonymous\r\n" :
        message = "USER " + address[USER] + "\r\n";
    reply = sendMessage(message.c_str(), socketDescConnect, "331");
    CHECK_REPLY;

    // send password
    (address[PASS] == EMPTY_STRING) ?
        message = "PASS secret\r\n" :
        message = "PASS " + address[PASS] + "\r\n";
    reply = sendMessage(message.c_str(), socketDescConnect, "230");
    CHECK_REPLY;

    // send directory / file listing as plain ASCII
    message = "TYPE A\r\n";
    reply = sendMessage(message.c_str(), socketDescConnect, "200");
    CHECK_REPLY;

    // enter to passive mode -> expect new socket connection to transfer data 
    message = "PASV\r\n";
    reply = sendMessage(message.c_str(), socketDescConnect, "227");
    CHECK_REPLY;

    // regex to split address from argument
    regex = ".*\\((\\d+),(\\d+),(\\d+),(\\d+),(\\d+),(\\d+)\\).*";
    addressParams = match(regex, reply);
    if (addressParams.empty()) {
        cerr << "Can't find IP address information in reply" << endl;
        return 1;
    }
    // get ip and port for transfer data from FTP server
    // TODO edit 
    address[HOST] = EMPTY_STRING;
    address[HOST].append(addressParams[1]);
    address[HOST].append(".");
    address[HOST].append(addressParams[2]);
    address[HOST].append(".");
    address[HOST].append(addressParams[3]);
    address[HOST].append(".");
    address[HOST].append(addressParams[4]);

    socketDescTransfer = connectToFtp(address[HOST].c_str(), 
            ((stringToInt(addressParams[5]) * 256) 
            + stringToInt(addressParams[6])));
    if (socketDescTransfer == -1) {
        return 1;
    }

    // print file list
    (address[PATH] == EMPTY_STRING) ?
        message = "LIST /\r\n" :
        message = "LIST " + address[PATH] + "\r\n";
    reply = sendMessage(message.c_str(), socketDescConnect, "150");
    CHECK_REPLY;


    // load and print files on ftp (TODO better solution)
    // TODO make function receiveMessage
    char   serverReply[BUFSIZE] = EMPTY_STRING;
    string serverReplyStr       = EMPTY_STRING;
    int    recvStatus           = 0;
    while(true) {
        // empty buffer
        memset(serverReply, 0, sizeof(serverReply));
        // check if there is data to receive
        if (isResponse(socketDescTransfer) <= 0) {
            cerr << "No data in socket to receive" << endl;
            return 1;
        }
        // receive reply
        if ((recvStatus = recv(socketDescTransfer, serverReply, BUFSIZE - 1, 0)) < 0) {
            cerr << "Recv failed" << endl;
            return 1;
        }
        if (recvStatus == 0) {
            break;
        }
        serverReplyStr.append(serverReply);
    }

    close(socketDescTransfer);

    // check if output file list is correctly loaded
    reply = sendMessage("", socketDescConnect, "226");
    CHECK_REPLY;

    cout << serverReplyStr;

    close(socketDescConnect);
    return 0;
}


/**
 * Function: isResponse
 * --------------------
 * Check if there are are some data
 * at socket to accept
 *
 * @param socketDesc socket descriptor
 * @return count of socket handles that are ready or
 *         0 if time limit expires
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


/**
 * Function: connectToFtp
 * ----------------------
 * Create new socket, convert hostname to IP address
 * and connect to FTP server
 *
 * @param hostname     hostname of FTP server
 * @param port         FTP server port
 * @return socketDesc  socket descriptor
 */

int connectToFtp(const char *hostname, int port)
{
    int socketDesc;
    struct hostent *he;
    struct sockaddr_in server;
    string ip = EMPTY_STRING;

    // create socket
    if ((socketDesc = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        cerr << "Can't create socket" << endl;
        return -1;
    }
    // convert hostname to ip
    if ((he = gethostbyname(hostname)) == NULL) {
        cerr << "Convert hostname to IP failed" << endl;
        return -1;
    }
    // connect socket to a server
    memcpy(&server.sin_addr, he->h_addr_list[0], he->h_length);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if (connect(socketDesc, (struct sockaddr *)&server, sizeof(server)) < 0) {
        cerr << "Connection error" << endl;
        return -1;
    }
    return socketDesc;
}


/**
 * Function: sendMessage
 * ---------------------
 * Send message to FTP server and expect answer 
 *
 * @param message      message that will be sent to FTP server
 * @param socket_desc  descriptor of socket that is connected to FTP server
 * @param code         code number that is expected in server reply
 * @return string      string containing server reply message
 */

string sendMessage(string message, int socket_desc, string code)
{
    char   serverReply[BUFSIZE] = EMPTY_STRING;
    string serverReplyStr       = EMPTY_STRING;

    // send message
    if (send(socket_desc, message.c_str(), strlen(message.c_str()), 0) < 0) {
        cerr << "Send failed" << endl;
        return EMPTY_STRING;
    }
    do {
        // empty buffer
        memset(serverReply, 0, sizeof(serverReply));
        // check if there is data to receive
        if (isResponse(socket_desc) <= 0) {
            cerr << "No data in socket to receive" << endl;
            return EMPTY_STRING;
        }
        // receive reply
        if (recv(socket_desc, serverReply, BUFSIZE - 1, 0) < 0) {
            cerr << "Recv failed" << endl;
            return EMPTY_STRING;
        }
        serverReplyStr.append(serverReply);
    } while (serverReplyStr.find(code) == string::npos);
    return serverReplyStr;
}


/**
 * Function: match
 * ---------------
 * Match regular expression and return matched group
 *
 * @param pattern regular expression that'll be used to match groups
 * @param source  source text on which will be regex search done
 * @return match  matched groups
 */

smatch match(string pattern, string source)
{
    smatch match;

    regex_search(source, match, regex(pattern));
    return match;
}


/**
 * Function: stringToInt
 * ---------------------
 * Convert string value to integer
 *
 * @param stringNum number in string
 * @return integer  number converted to int
 */

int stringToInt(string number)
{
    int integer;

    istringstream convert (number);
    convert >> integer;
    return integer;
}
