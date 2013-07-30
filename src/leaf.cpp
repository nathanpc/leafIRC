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
    if (argc == 2 && string(argv[1]) == "--help") {
        cerr << "Usage: leaf server_alias|server_url\n";
        return 1;
    }

    if (argc == 1) {
    	Config_Set config_set;
    	config_set.user();
    	config_set.save("user");
    	return 0;
    }

    Config config(true);
    config.load_user_config(argv[1]);

	IRC_Client client(config.server_location);
    client.setup_user(config.user_nick, config.user_username,
    	config.user_realname, config.user_password, config.channels);

    Splash::print();
    client.start_connection();
    
	return client.run();
}
