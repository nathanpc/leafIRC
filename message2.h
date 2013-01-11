/**
 *  message.h
 *
 *  @author Nathan Campos
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <string>
#include <vector>

class Message {
    private:
        std::string parse_server();
        std::string parse_hostname();
        std::string parse_command();
        std::string parse_arguments();
        std::string strip_end_newline(std::string line);

    public:
        std::string raw;
        std::string server;
        std::string username;
        std::string hostname;
        std::string cmd;
        std::vector<std::string> args;

        // Constructor.
        Message(const std::string& s);

        unsigned int get_reply_code();
        std::string get_nickname();
        std::string get_command();
        std::vector<std::string> get_command_args();
};

#endif
