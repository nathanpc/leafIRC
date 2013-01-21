/**
 *  leafIRC
 *  leaf.cpp
 *  The awesome lightweight IRC client for your command-line.
 *
 *  @author Nathan Campos
 */

#include <iostream>
#include <string>

#include "irc_client.h"
#include "splash.h"
#include "config.h"
using namespace std;

int main(int argc, char *argv[]) {
    Config config;

    IRC_Client client("localhost");
    //IRC_Client client("irc.freenode.net");
    
    client.setup_user(config.user_nick, config.user_username,
    	config.user_realname);

    Splash::print();
    client.start_connection();
    
    return client.run();
}
