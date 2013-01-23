/**
 *  irc_client.h
 *  IRC client helper library.
 *
 *  @author Nathan Campos
 */

#ifndef LEAF_IRC_CLIENT_H_
#define LEAF_IRC_CLIENT_H_

#include <string>
#include <vector>

#include "repl.h"
#include "channels.h"
#include "message.h"

class IRC_Client {
    private:
        int sd;  // socket_descriptor
        
        // True if we are connected
        bool connected;
        std::vector<std::string> autojoin_channels;

        bool send_data(std::string data);
        void message_handler(const char *buffer);
        void *handle_recv(void);
        static void *handle_recv_thread_helper(void *context);

    public:
        pthread_t thread;
        REPL repl;
        Channels channels;

        // Server configuration.
        std::string server;
        std::string port;
        std::string server_password;

        // Client configuration.
        std::string nick;
        std::string username;
        std::string realname;
        std::string nickserv;
        
        // Current message being processed
        Message message;
        
        std::vector<Message> log;
        
        // Constructor.
        IRC_Client(std::string _server, std::string _port = "6667",
        	std::string _server_pass = "");
        //~IRC_Client();

        void setup_user(std::string _nick, std::string _username,
        	std::string _realname, std::string _nickserv,
        	std::vector<std::string> _autojoin_channels);
        
        void start_connection();
        void close_connection();
        
        // Run the main loop for the client
        int run();
        
        // Return true if the client is connected to a server
        bool is_connected() const;
};

#endif	// LEAF_IRC_CLIENT_H_
