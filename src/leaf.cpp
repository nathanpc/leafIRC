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
    if (argc < 2) {
        cout << "Usage: leaf server_alias|server_url" << endl;
        return 1;
    }

    Config config;
    config.load_user_config(argv[1]);

    IRC_Client client(config.server_location);
    client.setup_user(config.user_nick, config.user_username,
    	config.user_realname, config.user_password);

    Splash::print();
    client.start_connection();
    
    return client.run();
}
