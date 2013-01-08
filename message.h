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
        std::string parse_msg();
        std::string strip_end_newline(std::string line);

    public:
        std::string raw;
        std::string server;
        std::string hostname;
        std::string msg;

        // Constructor.
        Message(char *_raw);

        std::string get_nickname();
        std::string get_command();
        std::vector<std::string> get_command_args();
};

#endif
