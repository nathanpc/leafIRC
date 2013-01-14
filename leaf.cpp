/**
 *  leafIRC
 *  leaf.cpp
 *  The awesome lightweight IRC client for your command-line.
 *
 *  @author Nathan Campos
 */

#include <iostream>
#include <string>
#include <curses.h>

#include "irc_client.h"
#include "splash.h"
#include "config.h"
using namespace std;

int main(int argc, char *argv[]) {
    Config config;
    IRC_Client client("irc.arcti.ca", "6667", "");
    //IRC_Client client("localhost", "6667", "");
    client.setup_user(config.user_nick, config.user_username, config.user_realname);

    Splash::print();
    client.start_connection();

    return 0;
}
