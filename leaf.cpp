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
    IRC_Client client("localhost", 6667, "");

    cout << "Server string: " << client.server << ":" << client.port << endl;
    return 0;
}
