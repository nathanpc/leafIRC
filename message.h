/**
 *  message.h
 *
 *  @author Nathan Campos
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <iosfwd>
#include <string>
#include <vector>

class Message {
    private:
        std::string _raw;
        std::string raw;
        std::string server;
        std::string nickname;
        std::string username;
        std::string hostname;
        std::string cmd;
        std::vector<std::string> args;
        
        bool parse_server();
        std::string parse_nickname();
        std::string parse_username();
        std::string parse_hostname();
        std::string parse_command();
        std::string parse_arguments();
        std::string strip_end_newline(std::string line);
        
        bool parse();

    public:
        // Constructor.
        Message(const char* s);
        
        // Output operator for output streams
        friend std::ostream& operator<<(std::ostream& out, const Message& m);

        unsigned int get_reply_code();
        
        std::string get_server();
        std::string get_nickname();
        std::string get_username();
        std::string get_hostname();
        std::string get_command();
        std::vector<std::string> get_command_args();
};

#endif
