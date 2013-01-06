/**
 *  irc_client.h
 *  IRC client helper library.
 *
 *  @author Nathan Campos
 */

#ifndef IRC_CLIENT_H_
#define IRC_CLIENT_H_

#include <string>

class IRC_Client {
    private:
        int socket_descriptor;

        bool send_data(std::string data);
        void message_handler(char *buffer);

    public:
        // Server configuration.
        std::string server;
        std::string port;
        std::string server_password;

        // Client configuration.
        std::string nick;
        std::string username;
        std::string realname;

        // Constructor.
        IRC_Client(std::string _server, std::string _port, std::string _server_pass);
        //~IRC_Client();

        void setup_user(std::string _nick, std::string _username, std::string _realname);
        void start_connection();
};

#endif
