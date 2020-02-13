#include <string>
#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <arpa/inet.h>
#include <regex>

#define HEAD 0
#define TAIL 1

///HTTP response codes
#define OK_200 "HTTP/1.1 200 OK\r\n"
#define CREATED_201 "HTTP/1.1 201 Created\r\n"
#define NOT_FOUND_404 "HTTP/1.1 404 Not Found\r\n"
#define CONFLICT_409 "HTTP/1.1 409 Conflict\r\n"
#define BAD_REQUEST_400 "HTTP/1.1 400 Bad Request\r\n"


#ifndef ISA2_ISAPROJECT_H
#define ISA2_ISAPROJECT_H

#endif //ISA2_ISAPROJECT_H

using namespace std;

/**
 * Splits string using the predefined delimiter
 * @param message 
 * @param select - HEAD or TAIL
 * @return either header or content of the message
 */
string splitString(const string& message, int select, const string& delimiter) {
    string result;
    if (select == HEAD) {
        result = message.substr(0, message.find(delimiter));
    } else if (select == TAIL) {
        result = message.substr(message.find(delimiter) + delimiter.length(), message.size());
    }
    return result;
}
