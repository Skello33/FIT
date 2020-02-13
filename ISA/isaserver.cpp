#include "isaproject.h"

struct board {
    string name;
    vector<string> content;
};

/**
 * Checks arguments
 * @param argc
 * @param argv
 * @return port number for server
 */
int checkArgs(int argc, char **argv) {
    int arg;
    int port = -1;
    if (argc < 2 || argc > 3) {
        cerr << "Error! Invalid arguments!" << endl;
        exit(EXIT_FAILURE);
    }

    while ((arg = getopt(argc, argv, "hp:")) != -1)
        switch (arg) {
            case 'p':
                if (argc != 3) {
                    fprintf(stderr, "Error! Invalid arguments!\n");
                    exit(EXIT_FAILURE);
                }
                port = (int) strtol(optarg, nullptr, 10);
                if (port == 0L){
                    cerr << "Bad port format!" << endl;
                    exit(EXIT_FAILURE);
                }
                break;
            case 'h':
                printf("Program help:\nUsage: ./isaserver -p <port>\n"); //todo insert help
                exit(EXIT_SUCCESS);
            default:
                printf("Error! Unexpected argument!\n");
                exit(EXIT_FAILURE);
        }
    if (port == -1) {
        cerr << "Error! No port specified!" << endl;
        exit(EXIT_FAILURE);
    }
    return port;
}

/**
 * Get name from command
 * @param command
 * @return string name
 */
string getName(const string &command) {
    string name = splitString(command, TAIL, " ");
    name = splitString(name, HEAD, " ");
    name = splitString(name, TAIL, "/");
    name = splitString(name, TAIL, "/");
    name = splitString(name, HEAD, "/");
    return name;
}

/**
 * Get ID from command
 * @param command
 * @return ID
 */
unsigned long getId(const string &command) {
    string name = splitString(command, TAIL, " ");
    name = splitString(name, HEAD, " ");
    name = splitString(name, TAIL, "/");
    name = splitString(name, TAIL, "/");
    name = splitString(name, TAIL, "/");
    return stoul(name);
}

/**
 * Finds position of specified board
 * @param boards
 * @param name - board <name>
 * @return position, true if found
 */
pair<unsigned long, bool> findPosition(const vector<board> &boards, const string &name) {
    pair<unsigned long, bool> result;
    result.second = false;
    //search boards vector for board with <name>
    for (result.first = 0; result.first < boards.size(); result.first++) {
        if (boards[result.first].name == name) {
            result.second = true;
            break;
        }
    }
    return result;
}

/**
 * Checks if entry with <id> exists in board <name>
 * @param boards
 * @param name
 * @param id
 * @return position of searched board, true if exists
 */
pair<unsigned long, bool> existsEntry(vector<board> boards, const string &name, unsigned id) {
    //find board by its name
    pair<unsigned long, bool> result = findPosition(boards, name);
    //check if board exists
    if (!result.second) {
        return result;
    }
    //check if entry id is in range of entries
    result.second = boards[result.first].content.size() >= id;
    return result;
}

/**
 * Iterate through board entries and put them in one string
 * @param boards
 * @param name
 * @return board content, false if no such board found
 */
pair<string, bool> printBoardContent(const vector<board> &boards, const string &name) {
    pair<string, bool> output;
    output.second = false;
    pair<unsigned long, bool> boardIndex = findPosition(boards, name);
    if (!boardIndex.second) {
        return output;
    }
    int counter = 1;
    for (const auto &entry : boards[boardIndex.first].content) {
        output.first.append(to_string(counter) + ". " + entry + "\n");
        counter++;
    }
    output.second = true;
    return output;
}

/**
 * Checks existence and correct form of content related headers
 * @param header
 * @param content
 * @return false if header is faulty, true if correct
 */
bool checkContentHeaders(const string &header, bool content){
    regex contentTypePattern("Content-Type: text/plain");
    regex contentLengthPattern("Content-Length: \\d");
    //check Content-Length
    regex contentLengthPattern0(".*Content-Length: 0.*");
    if (regex_search(header, contentLengthPattern0)) {
        return false;
    }
    bool result = false;
    if (content){
        //check existence of header fields regarding content
        if (regex_search(header, contentTypePattern)){
            if (regex_search(header, contentLengthPattern)) {
                result = true;
            }
        }
    }
    return result;
}

int main(int argc, char **argv) {
    int port = checkArgs(argc, argv);

    int serverSocket;
    int connectionSocket;
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Error! Socket wasn't created\n");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr *) &address, sizeof(address)) < 0) {
        fprintf(stderr, "Error! Socket bind failed!\n");
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, 1) < 0) {
        fprintf(stderr, "Error! Listen failed!\n");
        exit(EXIT_FAILURE);
    }

/*    ///time
//    ///https://www.tutorialspoint.com/cplusplus/cpp_date_time.htm
//    time_t now = time(nullptr);
//
//    // convert now to string form
//    char *dt = ctime(&now);
//
//    cout << "The local date and time is: " << dt << endl;
//
//    // convert now to tm struct for UTC
//    tm *gmtm = gmtime(&now);
//    dt = asctime(gmtm);
//    cout << "The UTC date and time is:" << dt << endl;
//    ///end of time*/

    printf("Listening on port : %d\n", port); //todo delete

    vector<board> boards;
    //server running cycle
    while (true) {
        //accept incoming connection
        if ((connectionSocket = accept(serverSocket, nullptr, nullptr)) < 0) {
            fprintf(stderr, "Error! Accept failed!\n");
            exit(EXIT_FAILURE);
        }
        //read incoming message
        char message[BUFSIZ];
        read(connectionSocket, message, BUFSIZ);
        string receivedMessage(message);

        //cut incoming message into header and content
        string delimiter = "\r\n\r\n";
        string header = splitString(receivedMessage, HEAD, delimiter);
        string content = splitString(receivedMessage, TAIL, delimiter);


        //variables for board handling
        string boardName; // name of the board
        unsigned id;    //entry ID
        string response = NOT_FOUND_404; //default server response
        pair<string, bool> responseContent; //string contains response content, bool determines if the desired content/board was found
        pair<unsigned long, bool> boardIndex;   //long contains the position of board in vector, bool determines if the desired content/board exists

        ///GET
        if (splitString(header, HEAD, " ") == "GET") {
            regex boardsPattern("\\/boards HTTP/1.1");  // /boards
            regex nameBoardsPattern("\\/board\\/([a-z]|[0-9]|[A-Z])+ HTTP/1.1");    // /board/name
            /// GET /boards
            if (regex_search(header, boardsPattern)) {
                //append board names to response content
                for (auto const &value: boards) {
                    responseContent.first.append(value.name + "\n");
                }
                //check if there is any response
                if (responseContent.first.empty()) {
                    response = NOT_FOUND_404;
                } else
                    response = OK_200;
            }
                /// GET /board/<name>
            else if (regex_search(header, nameBoardsPattern)) {
                boardName = getName(header);
                responseContent = printBoardContent(boards, boardName);
                //if board was found and is nonempty
                if (!(responseContent.first.empty())) {
                    if (responseContent.second) {
                        response = OK_200;
                    }
                }
            }
        }

            ///POST
        else if (splitString(header, HEAD, " ") == "POST") {
            regex boardsPattern("\\/boards\\/([a-z]|[0-9]|[A-Z])+ HTTP/1.1");   // /boards/name
            regex nameBoardPattern("\\/board\\/([a-z]|[0-9]|[A-Z])+ HTTP/1.1"); // /board/name
            /// POST /boards/<name>
            if (regex_search(header, boardsPattern)) {
                boardName = getName(header);
                board newBoard;
                boardIndex = findPosition(boards, boardName);
                if (boardIndex.second) {
                    //cout << "Board " << boardName << " already exists!!!" << endl; //todo finish this 409
                    response = CONFLICT_409;
                } else {
                    newBoard.name = boardName;
                    boards.push_back(newBoard);
                    response = CREATED_201;
                }
            }
                /// POST /board/<name>
            else if (regex_search(header, nameBoardPattern)) {
                if (checkContentHeaders(header, !content.empty())) {
                        boardName = getName(header);
                        //find corresponding board and insert content
                        boardIndex = findPosition(boards, boardName);
                        //if board wasn't found
                        if (!boardIndex.second) {
                            response = NOT_FOUND_404;
                        }
                            //if no content
                        else if (content.empty()){
                            response = BAD_REQUEST_400;
                        }
                            //insert content to board
                        else {
                            boards[boardIndex.first].content.emplace_back(content);
                            response = CREATED_201;
                        }

                } else
                    response = BAD_REQUEST_400;
            }
        }

            ///DELETE
        else if (splitString(header, HEAD, " ") == "DELETE") {
            regex boardsPattern("\\/boards\\/([a-z]|[0-9]|[A-Z])+ HTTP/1.1");   // /boards/name
            regex idBoardPattern(R"(\/board\/([a-z]|[0-9]|[A-Z])+\/[1-9]\d* HTTP/1.1)");
            ///DELETE /boards/<name>
            if (regex_search(header, boardsPattern)) {
                boardName = getName(header);
                boardIndex = findPosition(boards, boardName);
                //if requested board exists, delete it
                if (boardIndex.second) {
                    boards.erase(boards.begin() + boardIndex.first);
                    response = OK_200;
                }
            }
                ///DELETE /board/<name>/<id>
            else if (regex_search(header, idBoardPattern)) {
                boardName = getName(header);
                id = getId(header);
                //try to delete entry
                boardIndex = existsEntry(boards, boardName, id);
                if (boardIndex.second) {
                    boards[boardIndex.first].content.erase(boards[boardIndex.first].content.begin() + id - 1);
                    response = OK_200;
                }
            }
        }

            ///PUT
        else if (splitString(header, HEAD, " ") == "PUT") {
            if (checkContentHeaders(header, !content.empty())) {
                regex idBoardPattern(R"(\/board\/([a-z]|[0-9]|[A-Z])+\/[1-9]\d* HTTP/1.1)");
                if (regex_search(header, idBoardPattern)) {
                    boardName = getName(header);
                    id = getId(header);
                    boardIndex = existsEntry(boards, boardName, id);
                    //if selected board and entry exists, update it
                    if (boardIndex.second) {
                        if (!content.empty()) {
                            boards[boardIndex.first].content[id - 1] = content;
                            response = OK_200;
                        } else {
                            response = BAD_REQUEST_400;
                        }
                    }
                }
            } else
                response = BAD_REQUEST_400;
        }
        ///other request methods are responded with 404, which is set into response by default

        if (!responseContent.first.empty()) {
            ///if there is any content, insert header fields and content into response
            response.append(
                    "Content-Type: text/plain\r\nContent-Length: " + to_string(responseContent.first.length()) +
                    "\r\n\r\n" + responseContent.first);
        } else
            response.append("\r\n");

        //write response to client
        write(connectionSocket, response.c_str(), BUFSIZ);

        //close after communication
        close(connectionSocket);
    }

    //return 0;
}