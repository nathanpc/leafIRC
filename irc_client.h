/**
 *  irc_client.h
 *  IRC client helper library.
 *
 *  @author Nathan Campos
 */

#ifndef IRC_CLIENT_H_
#define IRC_CLIENT_H_

#include <string>
using namespace std;

class IRC_Client {
    public:
        // Server configuration.
        string server;
        int port;
        string server_password;

        // Client configuration.
        string nick;
        string username;
        string realname;

        // Constructor.
        IRC_Client(string _server, int _port, string _server_pass);
        //~IRC_Client();

        void setup_user(string _nick, string _username, string _realname);
};

#endif
