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

using namespace std;

int main(int argc, char *argv[]) {
    IRC_Client client("localhost", "6667", "");
    client.setup_user("leaf", "leafirc", "LeafIRC");

    client.start_connection();

    cout << "Server string: " << client.server << ":" << client.port << endl;
    return 0;
}
