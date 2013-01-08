/**
 *  message.cpp
 *
 *  @author Nathan Campos
 */

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "message.h"
using namespace std;

Message::Message(char *_raw) {
    raw = strip_end_newline(string(_raw));
    server = parse_server();
    hostname = parse_hostname();
    msg = parse_msg();
}

string Message::parse_server() {
    if (raw.at(0) == ':' && (raw.find("!") < raw.find("!"))) {
        return raw.substr(1, raw.find(" ") - 1);
    }

    return "";
}

string Message::parse_hostname() {
    if (raw.at(0) == ':' && (raw.find("!") < raw.find(" "))) {
        return raw.substr(1, raw.find(" ") - 1);
    }

    return "";
}

string Message::parse_msg() {
    if (raw.at(0) == ':') {
        return raw.substr(raw.find(" ") + 1);
    }

    return raw;
}

string Message::strip_end_newline(string line) {
    return line.erase(line.find("\r\n"), 2);
}

unsigned int Message::get_reply_code() {
    string reply_code_str = raw.substr(raw.find(" "), 4);
    unsigned int reply_code = 0;

    if (stringstream(reply_code_str) >> reply_code) {
        return reply_code;
    }

    return 0;
}

string Message::get_nickname() {
    return hostname.substr(0, hostname.find("!"));
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

    // Find :colon argument and combine the missing pieces.
    int colon_arg_location = -1;
    for (size_t i = 0; i < args.size(); i++) {
        if (colon_arg_location == -1) {
            // Find the first appearance.
            if (args.at(i).at(0) == ':') {
                colon_arg_location = i;
                args.at(i).erase(0, 1);
            }
        } else {
            args.at(colon_arg_location) += " " + args.at(i);
        }
    }

    return args;
}
