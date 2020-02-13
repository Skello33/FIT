#include <netdb.h>
#include "isaproject.h"

/**
 * Checks arguments
 * @param argc
 * @param argv
 * @return data gained from command line (host, port, command)
 */
vector<string> checkArgs(int argc, char **argv) {
    int arg;
    bool p = false, H = false;
    vector<string> argumentsData{};
    //printf("Argc is %d\n", argc);

    while ((arg = getopt(argc, argv, "hH:p:")) != -1) {
        switch (arg) {
            case 'p':
                argumentsData.emplace_back(optarg);
                if (!H) { //check correst order of arguments
                    printf("Error! Bad arguments order or missing mandatory argument!\n");
                    exit(EXIT_FAILURE);
                }
                p = true;
                break;
            case 'h':
                printf("Program help:\nUsage: ./isaclient -H <host> -p <port> <command>\n");
                exit(EXIT_SUCCESS);
            case 'H':
                argumentsData.emplace_back(optarg);
                H = true;
                break;
            default:
                printf("Error! Unexpected argument!\n");
                exit(EXIT_FAILURE);
        }
    }
    if (!p) {
        printf("Error! Missing mandatory argument!\n");
        exit(EXIT_FAILURE);
    }
    if (optind >= argc) {
        printf("Error! Missing command!\n");
        exit(EXIT_FAILURE);
    }
    while (optind < argc) {
        argumentsData.emplace_back(argv[optind++]);
    }
    return argumentsData;
}

/**
 * Transforms command line arguments to API commands
 * @param command
 * @param content
 * @return API commands
 */
string commandTransform(vector<string> command, string *content) {
    string result;
    // points to beginning of <command>
    unsigned int i = 2;
    regex namePattern("^([a-z]|[0-9]|[A-Z])+$");

    /// GET /boards
    if (command[i] == "boards") {
        if ((command.size()) > 3) {
            fprintf(stderr, "Bad command format!\n");
            exit(EXIT_FAILURE);
        }
        result.append("GET /");
        result.append(command[i]);
    }
        /// board ...
    else if (command[i] == "board") {
        // "board" command must be followed by 2 words
        if (command.size() != 5) {
            fprintf(stderr, "Bad command format!\n");
            exit(EXIT_FAILURE);
        }
        // add <name>
        if (command[i + 1] == "add") {
            if (regex_match(command[i + 2], namePattern)) {
                result.append("POST /boards/" + command[i + 2]);
            } else {
                fprintf(stderr, "Bad name format!\n");
                exit(EXIT_FAILURE);
            }
        }
            // delete <name>
        else if (command[i + 1] == "delete") {
            if (regex_match(command[i + 2], namePattern)) {
                result.append("DELETE /boards/" + command[i + 2]);
            } else {
                fprintf(stderr, "Bad name format!\n");
                exit(EXIT_FAILURE);
            }
        }
            // list <name>
        else if (command[i + 1] == "list") {
            if (regex_match(command[i + 2], namePattern)) {
                result.append("GET /board/" + command[i + 2]);
            } else {
                fprintf(stderr, "Bad name format!\n");
                exit(EXIT_FAILURE);
            }
        }
    }
        /// item ...
    else if (command[i] == "item") {
        // "item" command must be followed by 3 words
        if (command.size() != 6) {
            if (command.size() != 7 || command[i + 1] != "update") {
                fprintf(stderr, "Bad command format!\n");
                exit(EXIT_FAILURE);
            }
            // update <name> <id> <content>
            result.append("PUT /board/" + command[i + 2] + "/" + command[i + 3]);
            *content = command[i + 4];
        }
            // add <name> <content>
        else if (command[i + 1] == "add") {
            if (regex_match(command[i + 2], namePattern)) {
                result.append("POST /board/" + command[i + 2]);
            } else {
                fprintf(stderr, "Bad name format!\n");
                exit(EXIT_FAILURE);
            }
            *content = command[i + 3];
        }
            // delete <name> <id>
        else if (command[i + 1] == "delete") {
            if (regex_match(command[i + 2], namePattern)) {
                result.append("DELETE /board/" + command[i + 2] + "/" + command[i + 3]);
            } else {
                fprintf(stderr, "Bad name format!\n");
                exit(EXIT_FAILURE);
            }
        } else {
            fprintf(stderr, "Bad command format!\n");
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "Unknown command \"%s\" !\n", command[i].c_str());
        exit(EXIT_FAILURE);
    }
    return result;
}

int main(int argc, char **argv) {

    // parsed arguments
    vector<string> argumentsData = checkArgs(argc, argv);
    int port = (int) strtol(argumentsData[1].c_str(), nullptr, 10);
    string host = argumentsData[0];
    // <command>
    string command;
    // <content>
    string content;

    printf("\n");
    command = commandTransform(argumentsData, &content);


    int clientSocket;
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Error! Socket wasn't created\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    struct hostent *server;

    // make DNS resolution of the first parameter using gethostbyname()
    //solution from ISA /examples/examples.zip
    if ((server = gethostbyname(host.c_str())) == nullptr) { // check the first parameter
        cerr << "Error! Gethostbyname() failed!" << endl;
        exit(EXIT_FAILURE);
    }
    memcpy(&serverAddress.sin_addr,server->h_addr,server->h_length);

    string message = command + " HTTP/1.1\r\nHost: " + host + "\r\n";

    if (!(content.empty())) {
        string contentLength = to_string(content.length());
        //for Content-Length control
        //contentLength = to_string(0);
        message.append("Content-Type: text/plain\r\nContent-Length: " + contentLength + "\r\n\r\n" + content);
    } else message.append("\r\n");

    if (connect(clientSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        cerr << "Error! Connect failed!!" << endl;
        exit(EXIT_FAILURE);
    }
    //response buffer
    char response[BUFSIZ];

    //send message
    send(clientSocket, message.c_str(), BUFSIZ, 0);
    //read response
    read(clientSocket, response, BUFSIZ);
    //split response into header and content
    string header = response;
    string messageContent = splitString(header, TAIL, "\r\n\r\n");
    header = splitString(header, HEAD, "\r\n\r\n");

    //print header and content if nonempty
    cerr << header << endl;
    if (!messageContent.empty()) {
        cout << messageContent << endl;
    }
    return 0;
}