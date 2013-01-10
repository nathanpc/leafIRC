/**
 *  irc_client.cpp
 *  IRC client helper library.
 *
 *  @author Nathan Campos
 */

#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <pthread.h>

#include "irc_client.h"
#include "message.h"
#include "channels.h"
#include "color.h"

#define MAXDATASIZE 256
using namespace std;

IRC_Client::IRC_Client(string _server, string _port, string _server_password) {
    server = _server;
    port = _port;
    server_password = _server_password;
}

void IRC_Client::setup_user(string _nick, string _username, string _realname) {
    nick = _nick;
    username = _username;
    realname = _realname;
}

bool IRC_Client::send_data(string data) {
    const char *buffer = data.c_str();
    int len = strlen(buffer);
    int bytes_sent = send(socket_descriptor, buffer, len, 0);

    return bytes_sent;
}

void IRC_Client::message_handler(char *buffer) {
    Message message(buffer);
    string str_buffer(buffer);

    if (message.get_command() == "PING") {
        IRC_Client::send_data("PONG " + message.get_command_args().at(0) + "\r\n");
    } else {
        // Messages that need to be echoed.
        vector<string> arguments = message.get_command_args();
        bool echo = true;

        if (message.get_command() == "PRIVMSG") {
            if (arguments.at(0).at(0) == '#') {
                // Channel message.
                if (arguments.at(0) != "#" + channels.list.at(channels.current)) {
                    echo = false;
                }

                str_buffer = string(BOLDWHITE) + "<" + message.get_nickname() + "> " + string(RESET) + arguments.at(1) + "\r\n";
                channels.cache(arguments.at(0).substr(1), str_buffer);
            }
        } else if (message.get_reply_code()) {
            //unsigned int reply_code = message.get_reply_code();
            str_buffer = string(YELLOW) + "<server> " + string(RESET) + arguments.at(2) + "\r\n";
        } else if (message.get_command() == "JOIN") {
            channels.add(arguments.at(0).substr(1, arguments.at(0).find(":") - 1));
        }

        if (echo) {
            if (!repl.has_started) {
                cout << str_buffer;
            } else {
                repl.clear();
                cout << "\r" << str_buffer;
                repl.rewrite();
            }
        }
    }
}

void *IRC_Client::handle_recv(void) {
    // recv some data.
    int numbytes;
    char buffer[MAXDATASIZE];

    while (true) {
        numbytes = recv(socket_descriptor, buffer, MAXDATASIZE - 1, 0);
        buffer[numbytes] = '\0';
 
        message_handler(buffer);    

        if (numbytes == 0) {
            cout << "Connection terminated" << endl;
        }
    }
}

void IRC_Client::start_connection() {
    struct addrinfo hints, *servinfo;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int res;
    if ((res = getaddrinfo(server.c_str(), port.c_str(), &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(res));
    }

    // Setup the socket descriptor.
    if ((socket_descriptor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1) {
        perror("Error while assigning the socket descriptor");
    }

    // Connect!
    if (connect(socket_descriptor, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
        close(socket_descriptor);
        perror("Couldn't connect");
    }

    freeaddrinfo(servinfo);  // Free the server information, we don't need it anymore.
    pthread_create(&thread, NULL, &handle_recv_thread_helper, this);

    bool just_connected = true;
    while (true) {
        if (just_connected) {
            send_data("NICK " + nick + "\r\n");
            send_data("USER " + username + " 0 * :" + realname + "\r\n");  // TODO: Set user mode.

            just_connected = false;
        }

        repl.read();

        if (repl.string_is_ready) {
            if (!repl.external_command.empty()) {
                // Command.
                string command = repl.external_command.at(0);

                if (command == "switch") {
                    channels.current = channels.find_index(repl.external_command.at(1));

                    repl.clear();
                    cout << "\r" << BOLDBLUE << "Switching to #" << repl.external_command.at(1) << RESET << "\r\n";
                    cout << channels.load_cache(repl.external_command.at(1));
                    repl.current_str = "";
                } else if (command == "message") {
                    string send_msg = "PRIVMSG #" + channels.list.at(channels.current) + " :" + repl.current_str;

                    // TODO: Get nick.
                    string cache_msg = string(BOLDWHITE) + "<Me> " + string(RESET) + repl.current_str;
                    channels.cache(channels.list.at(channels.current), cache_msg);
                    send_data(send_msg.c_str());
                }
            } else {
                // Just send raw stuff.
                send_data(repl.current_str.c_str());
            }
        }
    }
}
