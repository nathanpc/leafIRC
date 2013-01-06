/**
 *  irc_client.cpp
 *  IRC client helper library.
 *
 *  @author Nathan Campos
 */

#include <iostream>
#include <string>

#include "irc_client.h"

using namespace std;

IRC_Client::IRC_Client(string _server, int _port, string _server_password) {
    server = _server;
    port = _port;
    server_password = _server_password;
}

void IRC_Client::setup_user(string _nick, string _username, string _realname) {
    nick = _nick;
    username = _username;
    realname = _realname;
}
