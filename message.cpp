/**
 *  message.cpp
 *
 *  @author Nathan Campos
 */

#include <iostream>
#include <string>
#include <vector>

#include "message.h"
using namespace std;

Message::Message(char *_raw) {
    raw = strip_end_newline(string(_raw));
    server = parse_server();
    msg = parse_msg();
}

string Message::parse_server() {
    if (raw.at(0) == ':') {
        return raw.substr(1, raw.find(" ") - 1);
    } else {
        return "";
    }
}

string Message::parse_msg() {
    if (raw.at(0) == ':') {
        return raw.substr(raw.find(" ") + 1);
    } else {
        return raw;
    }
}

string Message::strip_end_newline(string line) {
    return line.erase(line.find("\r\n"), 2);
}

string Message::get_command() {
    return msg.substr(0, msg.find(" "));
}

vector<string> Message::get_command_args() {
    vector<string> args;
    string remaining = msg;

    while (true) {
        size_t current_pos = remaining.find(" ") + 1;

        if (remaining != remaining.substr(current_pos)) {
            remaining = remaining.substr(current_pos);
            args.push_back(remaining.substr(0, remaining.find(" ")));
        } else {
            break;
        }
    }

    return args;
}
